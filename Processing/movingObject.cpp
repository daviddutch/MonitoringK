#include "movingObject.h"
#include <iostream>
#include <string>
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include "qdom.h"

using namespace std;

MovingObject::MovingObject(XnUserID pId, Generators& generators, xn::Player& player) :
    gen(generators),
    height(0),
    movingIn(false),
    movingOut(false),
    id(pId),
    g_player(player),
    nFrame(-1)
{
    printf("MovingObject(params)\n");
    player.TellFrame(gen.depth.GetName(), startFrameNo);
}
bool MovingObject::operator==(const MovingObject &movingObject) const {
    return movingObject.id == this->id;
}
void MovingObject::outputImage(Rect rect) {
    printf("outputImage()\n");
    //Make a copy of the complete ImageMap
    const XnRGB24Pixel* pImage = gen.image.GetRGB24ImageMap();
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
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();
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
    printf("update movingObject()\n");

    XnUInt32 nFrame;
    g_player.TellFrame(gen.depth.GetName(), nFrame);

    xn::SceneMetaData sceneMetaData;
    xn::DepthMetaData depthMetaData;

    gen.depth.GetMetaData(depthMetaData);

    gen.user.GetUserPixels(id, sceneMetaData);
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

    if(rect.bottom != -1)
    {
        computeMetrics();
        //printf("Rect %d: %d %d %d %d\n",id, rect.top, rect.left, rect.bottom , rect.right);

        //printf("ouptut style\n");
        //outputDepth(rect);
        //outputImage(rect);

        frames.push_back(Frame(nFrame, rect, com));


    }
    //frames[nFrame].init(nFrame, rect, com);
    //printf("end update pers\n");

}
void MovingObject::computeMetrics() {
    xn::SceneMetaData sceneMetaData;
    gen.user.GetUserPixels(id, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    XnPoint3D com;
    gen.user.GetCoM(id, com);
    XnPoint3D com2;
    gen.depth.ConvertRealWorldToProjective(1, &com, &com2);
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();
    if (com.Z != 0) {
        printf("real world : (%f, %f, %f)\n", com.X, com.Y, com.Z);
        printf("projective world : (%d, %d, %f)\n", (int)com2.X, (int)com2.Y, com2.Z);
        int i = (int)com2.X;
        int j = (int)com2.Y;

        //computes the with of the object on the center of gravity
        XnPoint3D lcom;
        XnPoint3D rcom;
        int nbOther = 5;
        bool found = false;
        for (int y=j; y<XN_VGA_Y_RES; y++){
            for(int x=i;x<XN_VGA_X_RES;x--){
                if (nbOther==0){
                    lcom.X = x;
                    lcom.Y = y;
                    lcom.Z = pDepthMap[y * XN_VGA_X_RES + x];
                    found = true;
                    break;
                }
                if (userPix[y * XN_VGA_X_RES + x ] != id) {
                    nbOther--;
                }
            }
            if (found) break;
        }
        nbOther = 5;
        found = false;
        for (int y=j; y<XN_VGA_Y_RES; y++){
            for(int x=i;x<XN_VGA_X_RES;x++){
                if (nbOther==0){
                    rcom.X = x;
                    rcom.Y = y;
                    rcom.Z = pDepthMap[y * XN_VGA_X_RES + x];
                    found = true;
                    break;
                }
                if (userPix[y * XN_VGA_X_RES + x ] != id) {
                    nbOther--;
                }
            }
            if (found) break;
        }
        gen.depth.ConvertProjectiveToRealWorld(1, &lcom, &lcom);
        gen.depth.ConvertProjectiveToRealWorld(1, &rcom, &rcom);
        printf("real world left : (%f, %f, %f)\n", lcom.X, lcom.Y, lcom.Z);
        printf("real world right : (%f, %f, %f)\n", rcom.X, rcom.Y, rcom.Z);
        float xd = rcom.X-lcom.X;
        float yd = rcom.Y-lcom.Y;
        float zd = rcom.Z-lcom.Z;
        float dist = sqrt(xd*xd + yd*yd + zd*zd);
        printf("real world distance : %f\n", dist);
        this->com.X = com.X;
        this->com.Y = com.Y;
        this->com.Z = com.Z;
    }
}

float MovingObject::getHeight() {

    return height;
}

float MovingObject::getHeightByFrame(int i)
{
    //get top/bottom point object

    Rect r = frames[i].getZone();
    printf("rect top:%f bottom:%f height:%f\n", r.top, r.bottom, r.top-r.bottom);
    return 0.0;
}

std::string MovingObject::getTypeMovement()
{
    float delta=0, firstZ = 0, lastZ = 0;
    int keyFrame = 0, firstFrame = 0, lastFrame = 0;
    bool first = true;
    for (int i=1;i<frames.size();i++){
        float z = frames[i].getCom().Z;
        if(z < 10000 && z > 0.1){
            if(first){
                firstZ = z;
                firstFrame = i;
            }
            lastZ = z;
            lastFrame = i;
            first = false;
        }
    }
    delta = lastZ - firstZ;
    keyFrame = (lastFrame - firstFrame) / 2;

    getHeightByFrame(keyFrame);

    string typeMovement;
    if(delta>0)
        typeMovement = "go out";
    else if(delta<0)
        typeMovement = "go in";
    else
        typeMovement = "unknown movement";

    printf("nbFrames:%d delta:%f firstZ:%f lastZ:%f typeMovement:%s\n",frames.size(), delta, firstZ, lastZ, typeMovement.c_str());

    return typeMovement;
}

void MovingObject::checkMovement(QDomDocument& doc, QDomElement& eventsNode)
{
    int currentMovement = -1 ; // 0 : unknown   1 : going out   2 : entering
    float lastZ = 0;
    int startFrameCurrentMovement = frames[0].getId();
    string typeMovement = "";

    for (int i=0;i<frames.size();i++){
        float z = frames[i].getCom().Z;
        if(z < 10000 && z > 0.1){   //possible value
            int move;
            if(lastZ - z > 0){
                move = 2;
                typeMovement = "entering";
            }else{
                move = 1;
                typeMovement = "going out";
            }

            if( (currentMovement != move && currentMovement !=-1) || i==(frames.size()-1) ){
                QDomElement eventNode = doc.createElement("event");
                eventNode.setAttribute("startFrameNo",startFrameCurrentMovement);
                eventNode.setAttribute("endFrameNo",frames[i].getId()-1);
                eventNode.setAttribute("typeEvent",typeMovement.c_str());
                eventsNode.appendChild(eventNode);

                startFrameCurrentMovement = frames[i].getId();
            }
            currentMovement = move;
            lastZ = z;
        }
    }
}

void MovingObject::toXML(QDomDocument& doc, QDomElement& sequenceNode) {
    XnUInt32 endFrameNo;
    g_player.TellFrame(gen.depth.GetName(), endFrameNo);

    printf("*** moving object xml %d***\n", id);
    QDomElement movingObjectNode = doc.createElement("movingObject");
    movingObjectNode.setAttribute("startFrameNo",startFrameNo);
    movingObjectNode.setAttribute("endFrameNo",endFrameNo);
    movingObjectNode.setAttribute("movingObjectType","");
    movingObjectNode.setAttribute("keyImage2d",0);
    movingObjectNode.setAttribute("keyImage3d",0);
    sequenceNode.appendChild(movingObjectNode);

    QDomElement eventsNode = doc.createElement("events");
    movingObjectNode.appendChild(eventsNode);

    checkMovement(doc, eventsNode);

    QDomElement framesNode = doc.createElement("frames");
    movingObjectNode.appendChild(framesNode);
    for (int i=0;i<=frames.size();i++){
        frames[i].toXML(doc, framesNode);
    }
}
