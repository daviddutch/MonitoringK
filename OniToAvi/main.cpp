#include <QtCore/QCoreApplication>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <GL/glut.h>
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <vlc/plugins/vlc_fourcc.h>


using namespace cv;
using namespace std;

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {		 														\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }

int main(int argc, char *argv[])
{
    XnStatus nRetVal;
    xn::Context g_Context;
    xn::ImageGenerator g_ImageGenerator;
    if (argc < 2)
    {
        return 0;
    }

    nRetVal = g_Context.Init();
    CHECK_RC(nRetVal, "Init");

    nRetVal = g_Context.OpenFileRecording(argv[1]);
    CHECK_RC(nRetVal, "InitFromOni");


    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE,g_ImageGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
            nRetVal = g_ImageGenerator.Create(g_Context);
            CHECK_RC(nRetVal, "Find image generator");
    }
    //Init Player
    xn::Player player;
    xn::NodeInfoList list;
    XnStatus rc = g_Context.EnumerateExistingNodes(list);
    if (rc == XN_STATUS_OK)
    {
        for (xn::NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it)
        {
                switch ((*it).GetDescription().Type)
                {
                case XN_NODE_TYPE_PLAYER:

                        (*it).GetInstance(player);
                }
        }
    }else{
        printf("Player: %s\n", xnGetStatusString(rc));
    }

    nRetVal = g_Context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");

    XnUInt32 nFrame, nFrameTot;
    const XnChar* strNodeName = g_ImageGenerator.GetName();

    player.GetNumFrames(strNodeName,nFrameTot);

    //INIT MOVIE
    CvVideoWriter *writer = 0;
    int isColor = 1;
    int fps     = 25;  // or 30
    int frameW  = 640; // 744 for firewire cameras
    int frameH  = 480; // 480 for firewire cameras
    writer=cvCreateVideoWriter("out.avi",CV_FOURCC('P','I','M','1'),
                                    fps,cvSize(frameW,frameH),isColor);

    //CV_FOURCC('P','I','M','1')
    //CV_FOURCC('D','I','V','3')
    //CV_FOURCC('D','I','V','X')
    //CV_FOURCC('M', 'P', '4', '2')
    //VLC_FOURCC('V','P','8','0')

    while (TRUE)
    {
        // Update to next frame
        nRetVal = g_Context.WaitOneUpdateAll(g_ImageGenerator);
        CHECK_RC(nRetVal, "WaitOneUpdateAll");

        //printf(".");

        const XnRGB24Pixel* pImage = g_ImageGenerator.GetRGB24ImageMap();
        XnRGB24Pixel ucpImage[XN_VGA_Y_RES*XN_VGA_X_RES];

        //Fillup the whole image
        for (int y=0; y<XN_VGA_Y_RES; y++) {
            for(int x=0;x<XN_VGA_X_RES;x++) {
                ucpImage[y * XN_VGA_X_RES + x ].nRed   = pImage[y * XN_VGA_X_RES + x ].nRed;
                ucpImage[y * XN_VGA_X_RES + x ].nGreen = pImage[y * XN_VGA_X_RES + x ].nGreen;
                ucpImage[y * XN_VGA_X_RES + x ].nBlue  = pImage[y * XN_VGA_X_RES + x ].nBlue;
            }
        }

        //output the image
        IplImage *rgbimg = cvCreateImageHeader(cvSize(640,480), 8, 3);
        cvSetData(rgbimg,ucpImage, 640*3);
        cvCvtColor(rgbimg,rgbimg,CV_RGB2BGR);

        cvWriteFrame(writer,rgbimg);

        player.TellFrame(strNodeName,nFrame);
        //finish at end of movie
        if(nFrame == nFrameTot -1){
            break;
        }
    }
    cvReleaseVideoWriter(&writer);

    // Clean up
    g_Context.Shutdown();

    return 1;
}
