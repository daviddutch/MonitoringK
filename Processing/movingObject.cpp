#include "movingObject.h"
#include <iostream>
#include <string>
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include "qdom.h"

MovingObject::MovingObject(XnUserID pId, xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator, xn::Player& player) :
    userGenerator(uGenerator),
    depthGenerator(dGenerator),
    imageGenerator(iGenerator),
    height(0),
    movingIn(false),
    movingOut(false),
    id(pId),
    g_player(player),
    nFrame(-1)
{
    printf("Person(params)\n");
    player.TellFrame(depthGenerator.GetName(), startFrameNo);
}
bool MovingObject::operator==(const MovingObject &movingObject) const {
    return movingObject.id == this->id;
}
void MovingObject::outputImage(Rect rect) {
    printf("outputImage()\n");
    //Make a copy of the complete ImageMap
    const XnRGB24Pixel* pImage = imageGenerator.GetRGB24ImageMap();
    XnRGB24Pixel ucpImage[XN_VGA_Y_RES*XN_VGA_X_RES];

    //Fillup the whole image
    for (int y=0; y<XN_VGA_Y_RES; y++) {
        for(int x=0;x<XN_VGA_X_RES;x++) {
            if (y>rect.top && y<rect.bottom && x>rect.left && x<rect.right) { //is it inside the rectangle containing the person
                ucpImage[y * XN_VGA_X_RES + x ].nRed   = pImage[y * XN_VGA_X_RES + x ].nRed;
                ucpImage[y * XN_VGA_X_RES + x ].nGreen = pImage[y * XN_VGA_X_RES + x ].nGreen;
                ucpImage[y * XN_VGA_X_RES + x ].nBlue  = pImage[y * XN_VGA_X_RES + x ].nBlue;
            } else {
                ucpImage[y * XN_VGA_X_RES + x ].nRed   = 255;
                ucpImage[y * XN_VGA_X_RES + x ].nGreen = 255;
                ucpImage[y * XN_VGA_X_RES + x ].nBlue  = 255;
            }
        }
    }

    //output the image
    IplImage *rgbimg = cvCreateImageHeader(cvSize(640,480), 8, 3);
    cvSetData(rgbimg,ucpImage, 640*3);
    cvCvtColor(rgbimg,rgbimg,CV_RGB2BGR);

    std::string filename("snapshot-");
    std::ostringstream file;
    std::string suffix("-rgb.png");

    file << filename << nFrame << "-" << id << "-" << movingIn << suffix;
    cv::imwrite(file.str(),rgbimg);

    cvReleaseImageHeader(&rgbimg);
    //delete[] ucpImage;
}

void MovingObject::outputDepth(Rect rect) {
    double alpha = 255.0/2048.0;
    printf("outputDepth()\n");
    //Get pointer on depthMap and prepare matrix to get the cut depth image
    const XnDepthPixel* pDepthMap = depthGenerator.GetDepthMap();
    printf("cvCreateMat\n");
    CvMat* depthMetersMat   = cvCreateMat(480, 640, CV_8UC1);

    printf("fillup\n");
    //Fillup the whole depth image
    for (int y=0; y<XN_VGA_Y_RES; y++) {
        for(int x=0;x<XN_VGA_X_RES;x++) {
            if (y>rect.top && y<rect.bottom && x>rect.left && x<rect.right) { //is it inside the rectangle containing the person
                depthMetersMat->data.ptr[y * XN_VGA_X_RES + x ] = pDepthMap[y * XN_VGA_X_RES + x] * alpha;
            } else {
                depthMetersMat->data.ptr[y * XN_VGA_X_RES + x ] =  255;
            }
        }
    }

    //output the image
    IplImage *kinectDepthImage;
    kinectDepthImage = cvCreateImage( cvSize(640,480),8,1);
    cvGetImage(depthMetersMat, kinectDepthImage);

    std::string filename("snapshot-");
    std::ostringstream file;
    std::string suffix("-depth.png");

    file << filename << nFrame << "-" << id << "-" << movingIn << suffix;
    cv::imwrite(file.str(),depthMetersMat);

    cvReleaseMat(&depthMetersMat);
}

void MovingObject::update() {
    //printf("update pers()\n");

    nFrame++;

    xn::SceneMetaData sceneMetaData;
    xn::DepthMetaData depthMetaData;

    depthGenerator.GetMetaData(depthMetaData);

    userGenerator.GetUserPixels(id, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    Rect rect;
    rect.top    = XN_VGA_Y_RES+1;
    rect.right  = -1;
    rect.bottom = -1;
    rect.left   = XN_VGA_X_RES+1;

    for (int y=0; y<XN_VGA_Y_RES; y++){
        for(int x=0;x<XN_VGA_X_RES;x++){
            if (userPix[y * XN_VGA_X_RES + x ] == id) {
                if (y<rect.top) rect.top=y;
                if (x>rect.right) rect.right=x;
                if (y>rect.bottom) rect.bottom=y;
                if (x<rect.left) rect.left=x;
            }
        }
    }
    //printf("ouptut style\n");
    //outputDepth(rect);
    //outputImage(rect);

    XnPoint3D com;
    userGenerator.GetCoM(id, com);
    XnPoint3D com2;
    depthGenerator.ConvertRealWorldToProjective(1, &com, &com2);
    if (com.Z != 0) {
       // printf("real world : (%f, %f, %f)\n", com.X, com.Y, com.Z);
        //printf("projective world : (%f, %f, %f)\n", com2.X, com2.Y, com2.Z);
        this->com.X = com.X;
        this->com.Y = com.Y;
        this->com.Z = com.Z;
    }
    frames.push_back(Frame(nFrame, rect, com));
    //frames[nFrame].init(nFrame, rect, com);
    //printf("end update pers\n");
}
float MovingObject::getHeight() {
    return height;
}
void MovingObject::toXML(QDomDocument& doc, QDomElement& sequenceNode) {
    XnUInt32 endFrameNo;
    g_player.TellFrame(depthGenerator.GetName(), endFrameNo);

    printf("*** moving object xml ***\n");
    QDomElement movingObjectNode = doc.createElement("movingObject");
    movingObjectNode.setAttribute("startFrameNo",startFrameNo);
    movingObjectNode.setAttribute("endFrameNo",endFrameNo);
    movingObjectNode.setAttribute("movingObjectType",0);
    movingObjectNode.setAttribute("keyImage2d",0);
    movingObjectNode.setAttribute("keyImage3d",0);
    sequenceNode.appendChild(movingObjectNode);

    QDomElement eventsNode = doc.createElement("events");
    movingObjectNode.appendChild(eventsNode);

    QDomElement framesNode = doc.createElement("frames");
    movingObjectNode.appendChild(framesNode);
    for (int i=0;i<=frames.size();i++){
        frames[i].toXML(doc, framesNode);
    }
}
