/****************************************************************************
*                                                                           *
*   Nite 1.3 - Players Sample                                               *
*                                                                           *
*   Author:     Oz Magal                                                    *
*                                                                           *
****************************************************************************/

/****************************************************************************
*                                                                           *
*   Nite 1.3	                                                            *
*   Copyright (C) 2006 PrimeSense Ltd. All Rights Reserved.                 *
*                                                                           *
*   This file has been provided pursuant to a License Agreement containing  *
*   restrictions on its use. This data contains valuable trade secrets      *
*   and proprietary information of PrimeSense Ltd. and is protected by law. *
*                                                                           *
****************************************************************************/

#include <QtCore/QCoreApplication>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <GL/glut.h>
#include "SceneDrawer.h"
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include <QStringList>
#include <qfile.h>
#include <QTextStream>
#include <qdom.h>
#include <string>
#include "sequence.h"
//#include "xmlwriter.h"

using namespace std;


#define MAX_USER 20

xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;
xn::ImageGenerator g_image;
xn::Player g_Player;

XnStatus rc = XN_STATUS_OK;
int nUser = 0;
bool hasUserInSight = false;
Sequence* sequence;

//XmlWriter* xmlWriter;
QDomDocument doc;
QDomElement movieNode;
string dateStart;
string fileName;
const XnChar* strNodeName;

#define SAMPLE_XML_PATH "Sample-User.xml"

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {		 														\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }

void createXML()
{
    XnUInt32 nFrameTot;
    g_Player.GetNumFrames(strNodeName,nFrameTot);

    //Create XML
    QDomImplementation impl = QDomDocument().implementation();
    // document with document type
    QString name = "kinectMovie";
    QString publicId = "-//XADECK//DTD Movie 1.0 //EN";
    QString systemId = "movie.xsd";
    doc = QDomDocument(impl.createDocumentType(name,publicId,systemId));

    // root node
    movieNode = doc.createElement("kinectMovie");
    movieNode.setAttribute("filenameONI",fileName.c_str());
    movieNode.setAttribute("totNumberFrames",nFrameTot);
    movieNode.setAttribute("framesPerSeconde",0);
    movieNode.setAttribute("startDateTime",dateStart.c_str());
    movieNode.setAttribute("endDateTime",0);
    doc.appendChild(movieNode);

    //XnUInt64 tmp;
    //g_Player.TellTimestamp(tmp);
    //printf("info player: %s TellTimestamp:%d \n", g_Player.GetInfo().GetDescription().Type, tmp);
}

void writeXML()
{
    //Write XML file
    char xmlFileName [50];
    sprintf (xmlFileName, "%s.xml", dateStart.c_str());
    QFile file( xmlFileName);
    if( !file.open(QFile::WriteOnly) )
      return ;
    QTextStream ts( &file );
    ts << doc.toString();
    file.close();
}

void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    printf("NewUser %d\n", user);
    if (!hasUserInSight){
        hasUserInSight = true;
        sequence = new Sequence(generator, g_DepthGenerator, g_image, g_Player);
    }
    nUser++;
}
void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
        printf("Lost user %d\n", user);
        nUser--;
        if (nUser==0) {
            sequence->toXML(doc, movieNode);
            hasUserInSight = false;
        }
}



#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480
#define START_CAPTURE_CHECK_RC(rc, what)												\
        if (nRetVal != XN_STATUS_OK)														\
{																					\
        printf("Failed to %s: %s\n", what, xnGetStatusString(rc));				\
        StopCapture();															\
        return ;																	\
}
void CleanupExit()
{
    if(hasUserInSight)
        sequence->toXML(doc, movieNode);
    g_Context.Shutdown();

    writeXML();

    exit (1);
}
void DrawProjectivePoints(XnPoint3D& ptIn, int width, double r, double g, double b)
{
        static XnFloat pt[3];

        pt[0] = ptIn.X;
        pt[1] = ptIn.Y;
        pt[2] = 0;
        glColor4f(r,
                g,
                b,
                1.0f);
        glPointSize(width);
        glVertexPointer(3, GL_FLOAT, 0, pt);
        glDrawArrays(GL_POINTS, 0, 1);

        glFlush();

}

// this function is called each frame
void glutDisplay (void)
{
        XnUInt32 nFrame, nFrameTot;
        g_Player.GetNumFrames(strNodeName,nFrameTot);
        g_Player.TellFrame(strNodeName,nFrame);

        //finish at end of movie
        if(nFrame == nFrameTot){
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
        g_DepthGenerator.GetMetaData(depthMD);

        glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);

        glDisable(GL_TEXTURE_2D);

        // Read next available data
        g_Context.WaitAndUpdateAll();

        // Process the data
        //DRAW
        g_DepthGenerator.GetMetaData(depthMD);
        g_UserGenerator.GetUserPixels(0, sceneMD);
        DrawDepthMap(depthMD, sceneMD, 0);

        if (hasUserInSight) sequence->update();

        glutSwapBuffers();

}


void glutIdle (void)
{
        //if (g_bQuit) {
        //        CleanupExit();
        //}

        // Display the frame
        glutPostRedisplay();
}

void glutKeyboard (unsigned char key, int x, int y)
{
        switch (key)
        {
        case 27:
                CleanupExit();
                break;
        }
}
void glInit (int * pargc, char ** argv)
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

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        fileName = argv[1];
        dateStart = fileName.substr(0,fileName.find("."));
    }else{
        printf("\nIntruder usage : intruder [ONI file]* \n");
        printf("       example : recorder 2011_03_29[15_14_59].oni\n");
        return 0;
    }

    rc = g_Context.Init();
    CHECK_RC(rc, "Init");

    rc = g_Context.OpenFileRecording(fileName.c_str());
    CHECK_RC(rc, "InitFromONI");

    rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(rc, "Find depth generator");

    rc = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if(rc!=XN_STATUS_OK){
      rc = g_UserGenerator.Create(g_Context);
      CHECK_RC(rc, "UserGenerator");
    }

    rc = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);
    CHECK_RC(rc, "Find image generator");

    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
            !g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    {
            printf("User generator doesn't support either skeleton or pose detection.\n");
            return XN_STATUS_ERROR;
    }
    XnBool isSupported = g_DepthGenerator.IsCapabilitySupported("AlternativeViewPoint");
    if(TRUE == isSupported)
    {
      XnStatus res = g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_image);
      if(XN_STATUS_OK != res)
      {
        printf("Getting and setting AlternativeViewPoint failed: %s\n", xnGetStatusString(res));
      }
    }else{
        printf("AlternativeViewPoint not supported");
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    //Init Player
    xn::NodeInfoList list;
    rc = g_Context.EnumerateExistingNodes(list);
    if (rc == XN_STATUS_OK)
    {
        for (xn::NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it)
        {
                switch ((*it).GetDescription().Type)
                {
                case XN_NODE_TYPE_PLAYER:

                        (*it).GetInstance(g_Player);
                }
        }
    }else{
        printf("Player: %s\n", xnGetStatusString(rc));
    }

    strNodeName = g_DepthGenerator.GetName();
    createXML();


    rc = g_Context.StartGeneratingAll();
    CHECK_RC(rc, "StartGenerating");

    XnCallbackHandle hUserCBs;
    g_UserGenerator.RegisterUserCallbacks(NewUser, LostUser, NULL, hUserCBs);



    glInit(&argc, argv);
    glutMainLoop();


}
