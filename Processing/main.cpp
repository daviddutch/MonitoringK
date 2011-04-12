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

#include "sequence.h"


#define MAX_USER 20

xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;
xn::ImageGenerator g_image;

int nUser = 0;
bool hasUserInSight = false;
Sequence* sequence;



void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    printf("NewUser %d\n", user);
    if (!hasUserInSight){
        hasUserInSight = true;
        sequence = new Sequence(6, generator, g_DepthGenerator, g_image);
    }
    nUser++;
}
void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
        printf("Lost user %d\n", user);
        nUser--;
        if (nUser==0) {
            sequence->toXML();
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
        g_Context.Shutdown();

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

#define SAMPLE_XML_PATH "Sample-User.xml"

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {																\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }


int main(int argc, char **argv)
{
    XnStatus rc = XN_STATUS_OK;

    if (argc > 1) {
        rc = g_Context.Init();
        CHECK_RC(rc, "Init");
        rc = g_Context.OpenFileRecording(argv[1]);
        if (rc != XN_STATUS_OK) {
            printf("Can't open recording %s: %s\n", argv[1], xnGetStatusString(rc));
            return 1;
        }
    } else {
        rc = g_Context.InitFromXmlFile(SAMPLE_XML_PATH);
        CHECK_RC(rc, "InitFromXml");
    }

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

    rc = g_Context.StartGeneratingAll();
    CHECK_RC(rc, "StartGenerating");

    XnCallbackHandle hUserCBs;
    g_UserGenerator.RegisterUserCallbacks(NewUser, LostUser, NULL, hUserCBs);



    glInit(&argc, argv);
    glutMainLoop();


}
