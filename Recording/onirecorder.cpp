#include <time.h>
#include "defs.h"
#include "onirecorder.h"


OniRecorder::OniRecorder(long duration, std::string destination)
{
    this->duration = duration;
    this->destination = destination;
}
void OniRecorder::init() {
    printf("init\n");
    nRetVal = context.Init();
    CHECK_RC(nRetVal, "Init");

    nRetVal = context.InitFromXmlFile(SAMPLE_XML_PATH);
    CHECK_RC(nRetVal, "InitFromXml");

    context.SetGlobalMirror(true); //mirror image

    nRetVal = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGenerator);
    CHECK_RC(nRetVal, "Find depth generator");

    nRetVal = context.FindExistingNode(XN_NODE_TYPE_IMAGE,imageGenerator);
    if (nRetVal != XN_STATUS_OK) {
            nRetVal = imageGenerator.Create(context);
            CHECK_RC(nRetVal, "Find image generator");
    }

    /*nRetVal = depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);
    if(nRetVal)
            printf("Failed to match Depth and RGB points of view: %s\n", xnGetStatusString(nRetVal));
    /*
    if(depthGenerator.IsCapabilitySupported("AlternativeViewPoint"))
            {
                    nRetVal = depthGenerator.GetAlternativeViewPointCap().SetViewPoint(imageGenerator);
                    CHECK_RC(nRetVal, "SetViewPoint for depth generator");
            }
    */
}
void OniRecorder::start() {
    printf("start\n");
    nRetVal = context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");

    char recordFile[256] = {0};
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    printf("filename\n");
    XnUInt32 size;
    xnOSStrFormat(recordFile, sizeof(recordFile)-1, &size,
             "%s%d-%02d-%02dT%02d_%02d_%02d.oni",
            *destination.c_str(), timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);


    recorder = new xn::Recorder;

    nRetVal = context.CreateAnyProductionTree(XN_NODE_TYPE_RECORDER, NULL, *recorder);
    START_CAPTURE_CHECK_RC(nRetVal, "Create recorder");

    nRetVal = recorder->SetDestination(XN_RECORD_MEDIUM_FILE, recordFile);
    START_CAPTURE_CHECK_RC(nRetVal, "set destination");

    nRetVal = recorder->AddNodeToRecording(depthGenerator, XN_CODEC_16Z_EMB_TABLES);
    START_CAPTURE_CHECK_RC(nRetVal, "add node Depth");

    nRetVal = recorder->AddNodeToRecording(imageGenerator, XN_CODEC_JPEG);
    START_CAPTURE_CHECK_RC(nRetVal, "add node Image");


    record();


    // Clean up
    context.Shutdown();
}

void OniRecorder::record() {
    printf("record\n");
    long current;
    long start = clock();

    while (TRUE) {
        printf(".");
        // Update to next frame
        nRetVal = context.WaitOneUpdateAll(depthGenerator);
        CHECK_RC(nRetVal, "WaitOneUpdateAll");
        current = (double)(clock() - start)/CLOCKS_PER_SEC; //current duration
        if(current >= duration) {
            endRecording();
            break;
        }
    }
}
void OniRecorder::endRecording() {
    printf("endRecording\n");
    if (recorder != NULL) {
        recorder->RemoveNodeFromRecording(depthGenerator);
        recorder->Unref();
        delete recorder;
    }
}
