#include "processor.h"
#include "defs.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <GL/glut.h>
#include "SceneDrawer.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include <QStringList>
#include <qfile.h>
#include <QTextStream>
#include <string>
#include <stdlib.h>

#include "tinyxml.h"


Processor *Processor::instance = NULL;

Processor::Processor() {
    nUser = 0;
    hasUserInSight = false;
}

Processor* Processor::getInstance() {
    if (NULL == instance) {
        instance =  new Processor();
    }
    return instance;
}

int Processor::start(int argc, char **argv) {

    //get the path of exe dir
    path = argv[0];
    int pos = path.find_last_of("/")+1;
    if (pos > 0 ){
        path = path.substr(0, pos);
    }else{
        path = "";
    }

    fileName = argv[1];         //ONI file
    bool isDisplay = false;     //display or not the depth scene
    if(argc>2)
        isDisplay=true;

    //get the name of file without path
    pos = fileName.find_last_of("/") +1;
    if (pos < 0 ) pos = 0;
    fileName = fileName.substr(pos);
    dateStart = fileName.substr(0,fileName.find("."));

    //Get the config
    std::string tmp =  path + "config.xml";
    const char *filename = tmp.c_str();
    TiXmlDocument config(filename);
    if (!config.LoadFile()) {
        printf("Error while loading config!\n");
        return 1;
    }
    TiXmlElement *root, *video, *faceDetection;
    root = config.FirstChildElement( "config" );
    int fps = 24;
    bool active = false;
    const char* cascadeFile;

    if (root) {
        video = root->FirstChildElement("video");
        fps = atoi(video->Attribute("fps2d"));
        faceDetection = root->FirstChildElement("faceDetection");
        active = faceDetection->Attribute("active");
        cascadeFile = faceDetection->Attribute("cascadeFile");
        tmp =  path + cascadeFile;
        cascadeFile = tmp.c_str();
    }
    MovingObject::init(active, cascadeFile);



    //create directory for media
    instance->dir = "movieData/"+fileName.substr(0,fileName.find_last_of("."));
    mkdir("movieData", 0777);
    mkdir((dir).c_str(), 0777);
    mkdir((dir+"/2D").c_str(), 0777);
    mkdir((dir+"/3D").c_str(), 0777);


    XnStatus rc = XN_STATUS_OK;
    xn::DepthGenerator g_DepthGenerator;
    xn::UserGenerator  g_UserGenerator;
    xn::ImageGenerator g_image;

    //Init context and all Node/Generator
    rc = context.Init();
    CHECK_RC(rc, "Init");

    context.SetGlobalMirror(true); //mirror image

    rc = context.OpenFileRecording(argv[1]);
    CHECK_RC(rc, "InitFromONI");

    rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(rc, "Find depth generator");

    rc = context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if(rc!=XN_STATUS_OK){
      rc = g_UserGenerator.Create(context);
      CHECK_RC(rc, "UserGenerator");
    }

    rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);
    CHECK_RC(rc, "Find image generator");

    initGenerator(g_UserGenerator, g_DepthGenerator);

    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
            !g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
        printf("User generator doesn't support either skeleton or pose detection.\n");
        return XN_STATUS_ERROR;
    }

    XnBool isSupported = g_DepthGenerator.IsCapabilitySupported("AlternativeViewPoint");
    if(TRUE == isSupported) {
      XnStatus res = g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_image);
      if(XN_STATUS_OK != res) {
        printf("Getting and setting AlternativeViewPoint failed: %s\n", xnGetStatusString(res));
      }
    } else {
        printf("AlternativeViewPoint not supported\n");
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    //Init Player
    xn::Player player;
    xn::NodeInfoList list;
    rc = context.EnumerateExistingNodes(list);
    if (rc == XN_STATUS_OK) {
        for (xn::NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it) {
                switch ((*it).GetDescription().Type) {
                    case XN_NODE_TYPE_PLAYER:
                        (*it).GetInstance(player);
                }
        }
    }else{
        printf("Player error: %s\n", xnGetStatusString(rc));
    }

    //Create a Generators contains all generators
    gen = new Generators(g_UserGenerator, g_DepthGenerator, g_image, player);

    strNodeName = g_image.GetName();

    createXML();    //Create a XMLDocument

    //Start the Nodes/Generators
    rc = context.StartGeneratingAll();
    CHECK_RC(rc, "StartGenerating");

    //Set callbacks functions : NewUser & LostUser
    XnCallbackHandle hUserCBs;
    g_UserGenerator.RegisterUserCallbacks(Processor::NewUser, Processor::LostUser, NULL, hUserCBs);


    XnUInt32 nFrame, nFrameTot;
    instance->gen->player.GetNumFrames(instance->strNodeName, nFrameTot);


    //Loop each frames with windows output or not
    if (!isDisplay){
        while(nFrame != nFrameTot -2){
            //update current frame id
            instance->gen->player.TellFrame(instance->strNodeName,nFrame);
            // Read next available data
            instance->context.WaitAndUpdateAll();
            //Update sequence if there is someone in the scene
            if (instance->hasUserInSight)
                instance->sequence->update();
        }
        CleanupExit();
    }else{
        //Start the GL to display Depth image
        glInit(&argc, argv);
        glutMainLoop();
    }
}

void Processor::createXML() {
    XnUInt32 nFrameTot;
    gen->player.GetNumFrames(strNodeName,nFrameTot);

    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild( decl );

    movieNode = new TiXmlElement("kinectMovie");
    movieNode->SetAttribute("id", fileName.substr(0,fileName.find_last_of(".")).c_str());
    movieNode->SetAttribute("totNumberFrames", nFrameTot);
    movieNode->SetAttribute("framesPerSeconde", 24);
    movieNode->SetAttribute("startDateTime", dateStart.c_str());
    movieNode->SetAttribute("endDateTime", 0);
    doc.LinkEndChild(movieNode);
}

void Processor::writeXML() {
    //Write XML file
    char xmlFileName [255];
    char buffer[255];
    sprintf (xmlFileName, "%s.xml", dateStart.c_str());
    getcwd(buffer, 255);

    std::ostringstream file;
    file << buffer << "/" << dateStart.c_str() << ".xml";
    doc.SaveFile(file.str().c_str());
}

void XN_CALLBACK_TYPE Processor::NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie) {
    //If there is no one in the scene, create a new sequence
    if (!instance->hasUserInSight){
        instance->hasUserInSight = true;
        instance->sequence = new Sequence((*(instance->gen)), instance->dir);
    }
    //if not add a user in the sequence
    instance->nUser++;
}
void XN_CALLBACK_TYPE Processor::LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie) {
    //sub a user in the sequence
    instance->nUser--;
    //If it was the last, close the sequence and write to XML
    if (instance->nUser==0) {
        instance->sequence->toXML(instance->movieNode);
        instance->hasUserInSight = false;
    }
}

void Processor::CleanupExit() {
    if(instance->hasUserInSight)
        instance->sequence->toXML(instance->movieNode);

    instance->writeXML();

    instance->context.Shutdown();

    exit (1);
}

void Processor::glutDisplay (void) {
        XnUInt32 nFrame, nFrameTot;
        instance->gen->player.GetNumFrames(instance->strNodeName,nFrameTot);
        instance->gen->player.TellFrame(instance->strNodeName,nFrame);

        //finish at end of movie
        if(nFrame == nFrameTot -1){
            CleanupExit();
            return;
        }

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup the OpenGL viewpoint
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        xn::SceneMetaData sceneMD;
        xn::DepthMetaData depthMD;
        instance->gen->depth.GetMetaData(depthMD);

        glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);

        glDisable(GL_TEXTURE_2D);

        bool display = false;

        if (!g_bPause)
        {
            if (g_bStep)
            {
                g_bStep = false;
                g_bPause = true;
            }
            display = true;
            // Read next available data
            instance->context.WaitAndUpdateAll();
        }
        // Process the data
        instance->gen->depth.GetMetaData(depthMD);
        instance->gen->user.GetUserPixels(0, sceneMD);
        DrawDepthMap(depthMD, sceneMD, 0);
        if(display)
            {
            if (instance->hasUserInSight) instance->sequence->update();
        }
        glutSwapBuffers();
}

void Processor::glutIdle (void)
{
        // Display the frame
        glutPostRedisplay();
}

void Processor::glutKeyboard (unsigned char key, int x, int y)
{
        switch (key)
        {
        case 27:
                CleanupExit();
                break;
        case 'd':
                if (g_bPause)
                    playNextFrame();
                break;
        case 'a':
                if (g_bPause)
                    playPrevFrame();
                break;
        case 'p':
                g_bPause = !g_bPause;
                break;
        }
}
void Processor::glInit (int * pargc, char ** argv)
{
        glutInit(pargc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize(GL_WIN_SIZE_X, GL_WIN_SIZE_Y);
        glutCreateWindow ("Prime Sense Nite Players Viewer");
        //glutFullScreen();
        glutSetCursor(GLUT_CURSOR_NONE);

        glutKeyboardFunc(glutKeyboard);
        glutDisplayFunc(glutDisplay);
        glutIdleFunc(glutIdle);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
}

void Processor::playNextFrame(){
    const XnChar* strNodeName = instance->gen->depth.GetName();

    XnUInt32 nFrame = 0;
    XnUInt32 nTotFrame = 0;
    if (instance->gen->player.GetNumFrames(strNodeName, nTotFrame) != XN_STATUS_OK)
        printf("ERROR nbFrames: %s\n", xnGetStatusString(0));
    if (instance->gen->player.TellFrame(strNodeName, nFrame) != XN_STATUS_OK)
        printf("ERROR nbFrames: %s\n", xnGetStatusString(0));

    if (instance->gen->player.SeekToFrame(strNodeName, 1, XN_PLAYER_SEEK_CUR) != XN_STATUS_OK)
             printf("ERROR seektoframe: %s\n", xnGetStatusString(0));

    g_bStep = true;
    g_bPause = false;
    glutPostRedisplay();
}

void Processor::playPrevFrame(){
    const XnChar* strNodeName = instance->gen->depth.GetName();

    XnUInt32 nFrame = 0;
    XnUInt32 nTotFrame = 0;
    if (instance->gen->player.GetNumFrames(strNodeName, nTotFrame) != XN_STATUS_OK)
        printf("ERROR nbFrames: %s\n", xnGetStatusString(0));
    if (instance->gen->player.TellFrame(strNodeName, nFrame) != XN_STATUS_OK)
        printf("ERROR nbFrames: %s\n", xnGetStatusString(0));

    if (instance->gen->player.SeekToFrame(strNodeName, -1, XN_PLAYER_SEEK_CUR) != XN_STATUS_OK)
             printf("ERROR seektoframe: %s\n", xnGetStatusString(0));

    g_bStep = true;
    g_bPause = false;
    glutPostRedisplay();
}
