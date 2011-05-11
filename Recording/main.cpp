#include <QtCore/QCoreApplication>
#include <stdio.h>
#include <stdlib.h>
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <string>
#include <QStringList>

using namespace std;


xn::Context g_Context;
xn::DepthGenerator g_DepthGenerator;
xn::ImageGenerator g_ImageGenerator;
xn::Recorder* g_pRecorder;

XnBool g_bCalibrated = false;
XnBool g_bRecord = false;

XnStatus nRetVal = XN_STATUS_OK;

string destination;

#define START_CAPTURE_CHECK_RC(rc, what)												\
        if (nRetVal != XN_STATUS_OK)														\
{																					\
        printf("Failed to %s: %s\n", what, xnGetStatusString(rc));				\
        StopCapture();															\
        return ;																	\
}
#define SAMPLE_XML_PATH "Sample-User.xml"

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {																\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return 0;													\
        }

static __inline__ unsigned long long rdtsc(void)
        {
          unsigned long long int x;
             __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
             return x;
        }

void StopCapture()
{
    g_bRecord = false;
    if (g_pRecorder != NULL)
    {
        g_pRecorder->RemoveNodeFromRecording(g_DepthGenerator);
        g_pRecorder->Unref();
        delete g_pRecorder;
    }
    g_pRecorder = NULL;
    printf("\nRecord turned %s\n", g_pRecorder ? "on" : "off");
}

void StartCapture()
{
    char recordFile[256] = {0};
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    XnUInt32 size;
    xnOSStrFormat(recordFile, sizeof(recordFile)-1, &size,
             "%s%d-%02d-%02dT%02d_%02d_%02d.oni",
            destination.c_str(), timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    if (g_pRecorder != NULL)
    {
            StopCapture();
    }
    g_pRecorder = new xn::Recorder;

    nRetVal = g_Context.CreateAnyProductionTree(XN_NODE_TYPE_RECORDER, NULL, *g_pRecorder);
    START_CAPTURE_CHECK_RC(nRetVal, "Create recorder");

    nRetVal = g_pRecorder->SetDestination(XN_RECORD_MEDIUM_FILE, recordFile);
    START_CAPTURE_CHECK_RC(nRetVal, "set destination");
    nRetVal = g_pRecorder->AddNodeToRecording(g_DepthGenerator, XN_CODEC_16Z_EMB_TABLES);
    START_CAPTURE_CHECK_RC(nRetVal, "add node Depth");
    nRetVal = g_pRecorder->AddNodeToRecording(g_ImageGenerator, XN_CODEC_JPEG);
    START_CAPTURE_CHECK_RC(nRetVal, "add node Image");
    g_bRecord = true;
    printf("\nRecord turned %s\n", g_pRecorder ? "on" : "off");
}


int main(int argc, char *argv[])
{
    long duration;              //recording duration
    long start, current;        //use to duration calcul
    int hour = 0, minute = 0;   //use to start time
    bool isTimeStart = false;
    time_t rawtime;
    struct tm *timeinfo;
    QStringList list;

    if (argc > 1)
    {
        switch (argc)
        {
        case 4:
            isTimeStart = true;
            //Split the string to hour and minute var
            list = QString(argv[3]).split(":");
            if (list.count()>2){
                printf("error while parsing time \n");
                return 0;
            }
            hour =  atoi(list.takeFirst().toStdString().c_str());
            minute =  atoi(list.takeFirst().toStdString().c_str());
        case 3:
            destination = argv[2];
        case 2:
            duration = atoi(argv[1]);
            break;
        }
    }
    else
    {
        printf("\nRecorder usage : recorder [duration sec]* [destination ONI] [Time start] \n");
        printf("       example : recorder 120 /home/user/ 12:30 \n");
        printf("       example : recorder 60 \n");
        return 0;
    }

    printf("the recording will start ");
    if(isTimeStart)
        printf("at %02d:%02d", hour, minute);
    else
        printf("now");
    printf(" during %d sec and will be saved in ", duration);
    if(destination != "")
        printf("%s", destination.c_str());
    else
        printf("the same directory");
    printf("\n");

    //wait until time start is now or not specify
    while(true){
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("current time: %02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min);
        if(!isTimeStart || (timeinfo->tm_hour == hour && timeinfo->tm_min == minute) )
            break;
        sleep(30);
    }

    nRetVal = g_Context.Init();
    CHECK_RC(nRetVal, "Init");

    //TEMP : Virtual Kinect ONI file
    //nRetVal = g_Context.OpenFileRecording("Captured5.oni");
    nRetVal = g_Context.InitFromXmlFile(SAMPLE_XML_PATH);
    CHECK_RC(nRetVal, "InitFromXml");

    //g_Context.SetGlobalMirror(true); //mirror image

    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(nRetVal, "Find depth generator");

    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_IMAGE,g_ImageGenerator);
    if (nRetVal != XN_STATUS_OK)
    {
            nRetVal = g_ImageGenerator.Create(g_Context);
            CHECK_RC(nRetVal, "Find image generator");
    }

    /*nRetVal = g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_ImageGenerator);
    if(nRetVal)
            printf("Failed to match Depth and RGB points of view: %s\n", xnGetStatusString(nRetVal));
    /*
    if(g_DepthGenerator.IsCapabilitySupported("AlternativeViewPoint"))
            {
                    nRetVal = g_DepthGenerator.GetAlternativeViewPointCap().SetViewPoint(g_ImageGenerator);
                    CHECK_RC(nRetVal, "SetViewPoint for depth generator");
            }
    */



    nRetVal = g_Context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");

    start = clock();

    StartCapture();
    while (TRUE)
    {
        printf(".");
        // Update to next frame
        nRetVal = g_Context.WaitOneUpdateAll(g_DepthGenerator);
        CHECK_RC(nRetVal, "WaitOneUpdateAll");
        current = (double)(clock() - start)/CLOCKS_PER_SEC; //current duration
        if(current >= duration){
            StopCapture();
            break;
        }
    }

    // Clean up
    g_Context.Shutdown();

    return 1;
}
