#include "movingObject.h"
#include <iostream>
#include <string>
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include "qdom.h"

using namespace std;

MovingObject::MovingObject(XnUserID pId, Generators& generators) :
    gen(generators),
    height(0),
    movingIn(false),
    movingOut(false),
    id(pId),
    nFrame(-1)
{
    printf("MovingObject(params)\n");
    gen.player.TellFrame(gen.depth.GetName(), startFrameNo);
}
bool MovingObject::operator==(const MovingObject &movingObject) const {
    return movingObject.id == this->id;
}
XnPoint3D MovingObject::getCom()
{
    return com;
}
int MovingObject::getId()
{
    return id;
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
    //printf("update movingObject()\n");

    XnUInt32 nFrame;
    gen.player.TellFrame(gen.depth.GetName(), nFrame);

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
    gen.depth.ConvertProjectiveToRealWorld(1, &com, &com2);
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();
    if (com.Z != 0) {
        //printf("real world : (%f, %f, %f)\n", com.X, com.Y, com.Z);
        //printf("projective world : (%d, %d, %f)\n", (int)com2.X, (int)com2.Y, com2.Z);
        int i = (int)com2.X;
        int j = (int)com2.Y;

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
        if(lcom.X > 0.1 && rcom.X > 0.1){
            printf("real world %d left : (%f, %f, %f)\n", id, lcom.X, lcom.Y, lcom.Z);
            printf("real world %d right : (%f, %f, %f)\n", id, rcom.X, rcom.Y, rcom.Z);
            float xd = rcom.X-lcom.X;
            float yd = rcom.Y-lcom.Y;
            float zd = rcom.Z-lcom.Z;
            //float dist = sqrt(xd*xd + yd*yd + zd*zd);
            float dist = getDistance(lcom, rcom);
            printf("real world %d distance : %f\n", id, dist);
        }
        this->com.X = com2.X;
        this->com.Y = com2.Y;
        this->com.Z = com2.Z;

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

float MovingObject::checkDistance()
{
    float distance = 0.0;
    XnPoint3D current;
    XnPoint3D last = frames[0].getCom();
    for (int i=1;i<frames.size();i++){
        current = frames[i].getCom();
        distance += getDistance(last, current);
        last = current;
    }
    printf("distance tot %d: %f\n", id, distance);

    return distance;
}

float MovingObject::getDistance(XnPoint3D p1, XnPoint3D p2)
{
    return sqrt(pow(p1.X-p2.X,2) + pow(p1.Y-p2.Y,2) + pow(p1.Z-p2.Z,2));
}

void MovingObject::toXML(QDomDocument& doc, QDomElement& sequenceNode) {
    XnUInt32 endFrameNo;
    gen.player.TellFrame(gen.depth.GetName(), endFrameNo);

    checkDistance();
    outputImagesKey();

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

void MovingObject::outputImagesKey()
{
    int key = frames[frames.size()/2].getId();
    printf("key: %d\n", key);
    gen.player.SeekToFrame(gen.depth.GetName(),key, XN_PLAYER_SEEK_SET);
    XnUInt32 no;
    gen.player.TellFrame(gen.depth.GetName(), no);
    printf("seek: %d\n", no);

    Rect rect;
    rect.bottom    = XN_VGA_Y_RES;
    rect.left      = 0;
    rect.top       = 0;
    rect.right     = XN_VGA_X_RES;
    outputImage(rect);
    //outputDepth(rect);
}
