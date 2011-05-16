#include "movingObject.h"
#include <iostream>
#include <string>
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include "tinyxml.h"

using namespace std;

int MovingObject::next_id = 0;

MovingObject::MovingObject(XnUserID pId, Generators& generators, std::string d) :
    gen(generators),
    dir(d),
    xnUserId(pId)
{
    next_id++;
    id = next_id;
    //printf("MovingObject(%d, %d)\n", id, xnUserId);
    std::ostringstream file1, file2;
    file1 << dir << "/2D/keyimage-" << id << ".png";
    file2 << dir << "/3D/keyimage-" << id << ".png";
    file2d = file1.str();
    file3d = file2.str();
    gen.player.TellFrame(gen.depth.GetName(), startFrameNo);
    stableHeight = 20;
    validWidthCount = MAX_VALID_WIDTH_COUNT;
    state = IN_SIGHT;
}
bool MovingObject::operator==(const MovingObject &movingObject) const {
    return movingObject.xnUserId == this->xnUserId;
}
MovingObject& MovingObject::operator=(const MovingObject& rhs) {
    if (&rhs == this) return *this;

    return *this;
}

XnPoint3D MovingObject::getCom() {
    return com;
}
XnUserID MovingObject::getXnId() {
    return xnUserId;
}
void MovingObject::setXnId(XnUserID xnUserId) {
    this->xnUserId = xnUserId;
}
Metric MovingObject::getMetric() {
    return metric;
}
void MovingObject::outputImage(Rect rect) {
    XnUInt32 nFrame;  //TODO: checkout on the nFrame in this class
    gen.player.TellFrame(gen.depth.GetName(), nFrame);

    std::ostringstream file;
    file << "snapshot-" << nFrame << "-" << id << "-rgb.png";
    printf("Rect %d (%d;%d;%d;%d)\n", id, rect.top, rect.right, rect.bottom, rect.left);
    outputImage(rect, file.str());
}
void MovingObject::outputImage(Rect rect, std::string file) {
    //compute the inside rect to draw rectangle
    Rect rect2;
    rect2.top    = rect.top+2;
    rect2.right  = rect.right-2;
    rect2.bottom = rect.bottom-2;
    rect2.left   = rect.left+2;

    //Make a copy of the complete ImageMap
    const XnRGB24Pixel* pImage = gen.image.GetRGB24ImageMap();
    XnRGB24Pixel ucpImage[XN_VGA_Y_RES*XN_VGA_X_RES];

    //Fillup the whole image
    for (int y=0; y<XN_VGA_Y_RES; y++) {
        for(int x=0;x<XN_VGA_X_RES;x++) {
            if (y>=rect.top && y<=rect.bottom && x>=rect.left && x<=rect.right) { //is it inside the rectangle containing the person
                if (y>=rect2.top && y<=rect2.bottom && x>=rect2.left && x<=rect2.right) {
                    ucpImage[y * XN_VGA_X_RES + x ].nRed   = pImage[y * XN_VGA_X_RES + x ].nRed;
                    ucpImage[y * XN_VGA_X_RES + x ].nGreen = pImage[y * XN_VGA_X_RES + x ].nGreen;
                    ucpImage[y * XN_VGA_X_RES + x ].nBlue  = pImage[y * XN_VGA_X_RES + x ].nBlue;
                }else{
                    ucpImage[y * XN_VGA_X_RES + x ].nRed   = 139;
                    ucpImage[y * XN_VGA_X_RES + x ].nGreen = 0;
                    ucpImage[y * XN_VGA_X_RES + x ].nBlue  = 0;
                }
            } else {
                ucpImage[y * XN_VGA_X_RES + x ].nRed   = pImage[y * XN_VGA_X_RES + x ].nRed;
                ucpImage[y * XN_VGA_X_RES + x ].nGreen = pImage[y * XN_VGA_X_RES + x ].nGreen;
                ucpImage[y * XN_VGA_X_RES + x ].nBlue  = pImage[y * XN_VGA_X_RES + x ].nBlue;
            }
        }
    }

    //output the image
    IplImage *rgbimg = cvCreateImageHeader(cvSize(640,480), 8, 3);
    cvSetData(rgbimg,ucpImage, 640*3);
    cvCvtColor(rgbimg,rgbimg,CV_RGB2BGR);

    //cvSaveImage(file.str().c_str(),rgbimg);
    cv::imwrite(file, rgbimg);

    chmod(file.c_str(), 0777);

    cvReleaseImageHeader(&rgbimg);
    //delete[] ucpImage;
}

void MovingObject::outputDepth(Rect rect) {
    XnUInt32 nFrame;  //TODO: checkout on the nFrame in this class
    gen.player.TellFrame(gen.depth.GetName(), nFrame);

    std::ostringstream file;
    file << "snapshot-" << nFrame << "-" << id << "-depth.png";

    outputDepth(rect, file.str());
}
void MovingObject::outputDepth(Rect rect, std::string file) {
    double alpha = 255.0/2048.0;
    Rect rect2;
    rect2.top    = rect.top+2;
    rect2.right  = rect.right-2;
    rect2.bottom = rect.bottom-2;
    rect2.left   = rect.left+2;

    //Get pointer on depthMap and prepare matrix to get the cut depth image
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();
    //printf("cvCreateMat\n");
    CvMat* depthMetersMat   = cvCreateMat(480, 640, CV_8UC1);

    //printf("fillup\n");
    //Fillup the whole depth image
    for (int y=0; y<XN_VGA_Y_RES; y++) {
        for(int x=0;x<XN_VGA_X_RES;x++) {
            if (y>=rect.top && y<=rect.bottom && x>=rect.left && x<=rect.right) { //is it inside the rectangle containing the person
                if (y>=rect2.top && y<=rect2.bottom && x>=rect2.left && x<=rect2.right) {
                    depthMetersMat->data.ptr[y * XN_VGA_X_RES + x ] = pDepthMap[y * XN_VGA_X_RES + x] * alpha;
                }else{
                    depthMetersMat->data.ptr[y * XN_VGA_X_RES + x ] = 255;
                }
            } else {
                depthMetersMat->data.ptr[y * XN_VGA_X_RES + x ] = pDepthMap[y * XN_VGA_X_RES + x] * alpha;
            }
        }
    }

    //output the image
    IplImage *kinectDepthImage;
    kinectDepthImage = cvCreateImage( cvSize(640,480),8,1);
    cvGetImage(depthMetersMat, kinectDepthImage);

    cv::imwrite(file,depthMetersMat);

    cvReleaseMat(&depthMetersMat);
}

void MovingObject::update(Metric newMetric) {
   // printf("update movingObject(%d)\n", id);

    XnUInt32 nFrame;
    gen.player.TellFrame(gen.depth.GetName(), nFrame);

    xn::SceneMetaData sceneMetaData;
    xn::DepthMetaData depthMetaData;

    gen.depth.GetMetaData(depthMetaData);

    gen.user.GetUserPixels(id, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    gen.user.GetCoM(xnUserId, com);

    if (com.Z != 0){ //TODO: is that correct, working ?????
        Rect rect;
        rect.top    = XN_VGA_Y_RES+1;
        rect.right  = -1;
        rect.bottom = -1;
        rect.left   = XN_VGA_X_RES+1;

        for (int y=0; y<XN_VGA_Y_RES; y++){
            for(int x=0;x<XN_VGA_X_RES;x++){
                if (userPix[y * XN_VGA_X_RES + x ] == xnUserId) {
                    if (y<rect.top) rect.top=y;
                    if (x>rect.right) rect.right=x;
                    if (y>rect.bottom) rect.bottom=y;
                    if (x<rect.left) rect.left=x;
                }
            }
        }
        computeComColor();

        frames.push_back(Frame(nFrame, rect, com));

        float evolvHeight = abs(metric.height-this->metric.height)/metric.height;
        if (evolvHeight<0.1 && stableHeight>0){
            stableHeight--;
            if (stableHeight==0)
                outputKeyImages();
        }else if (stableHeight>0){
            stableHeight=20;
        }
        metric = newMetric;
    }else{
        //xnUserId = 0;
        printf("should not happen !!!!\n");
    }
    if (nFrame==startFrameNo)
        outputKeyImages();
}

//Source : http://www.compuphase.com/cmetric.htm
double ColourDistance(XnRGB24Pixel c1, XnRGB24Pixel c2) {
  long rmean = ( (long)c1.nRed + (long)c2.nRed ) / 2;
  long r = (long)c1.nRed - (long)c2.nRed;
  long g = (long)c1.nGreen - (long)c2.nGreen;
  long b = (long)c1.nBlue - (long)c2.nBlue;
  return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
}
//TODO: checkout on x and y out of the frame (segmentation fault)
void MovingObject::computeComColor(){
    XnPoint3D com;
    gen.user.GetCoM(xnUserId, com);
    XnPoint3D com2;
    gen.depth.ConvertRealWorldToProjective(1, &com, &com2);
    int zoneSize = 2;

    if (com.Z != 0) {
        int i = (int)com2.X;
        int j = (int)com2.Y;

        const XnRGB24Pixel* pImage = gen.image.GetRGB24ImageMap();

        XnRGB24Pixel average;
        int red = 0;
        int green = 0;
        int blue = 0;
        int nPixel = 0;
        //printf("x, y : (%d, %d)\n", i, j);
        for (int x=i-zoneSize; x<i+zoneSize; x++){
            for (int y=j-zoneSize; y<j+zoneSize; y++){
                red   += pImage[y * XN_VGA_X_RES + x ].nRed;
                green += pImage[y * XN_VGA_X_RES + x ].nGreen;
                blue  += pImage[y * XN_VGA_X_RES + x ].nBlue;
                nPixel++;
            }
        }
        average.nRed   = red/nPixel;
        average.nGreen = green/nPixel;
        average.nBlue  = blue/nPixel;

        if (comColor.nRed==0 && comColor.nGreen==0 && comColor.nBlue==0){

        }

        double dist = ColourDistance(comColor, average);

        //printf("color dist with special is : %f\n", dist);

        dist = sqrt(pow(comColor.nRed-average.nRed,2) + pow(comColor.nGreen-average.nGreen,2) + pow(comColor.nBlue-average.nBlue,2));

        //printf("color dist with normal is : %f\n", dist);

        comColor.nRed  = average.nRed;
        comColor.nGreen = average.nGreen;
        comColor.nBlue  = average.nBlue;

        //printf("Com color : (%d, %d, %d)\n", average.nRed, average.nGreen, average.nBlue);

        /*
        Rect rect;
        rect.bottom    = XN_VGA_Y_RES;
        rect.left      = 0;
        rect.top       = 0;
        rect.right     = XN_VGA_X_RES;
        outputImage(rect);
        */

        //if (dist>50){
            Rect rect;
            rect.top       = j-zoneSize;
            rect.right     = i+zoneSize;
            rect.bottom    = j+zoneSize;
            rect.left      = i-zoneSize;
            XnUInt32 nFrame;  //TODO: checkout on the nFrame in this class
            gen.player.TellFrame(gen.depth.GetName(), nFrame);

            std::ostringstream file;
            file << dir << "/3D/com-" << nFrame << "-" << id << "-rgb.png";
            try
            {
                //outputDepth(rect, file.str());
            }
            catch( cv::Exception& e )
            {
                std::cout << "Exception while writting file : " << e.err << std::endl;
            }

        //}

        /*
        for (int y=0; y<XN_VGA_Y_RES; y++){
            for(int x=0;x<XN_VGA_X_RES;x++){
                if (userPix[y * XN_VGA_X_RES + x ] == id) {
                    if (y<rect.top) rect.top=y;
                    if (x>rect.right) rect.right=x;
                    if (y>rect.bottom) rect.bottom=y;
                    if (x<rect.left) rect.left=x;
                }
            }
        }*/


    }
}


std::vector<Frame> MovingObject::getFrames(){
    return frames;
}
/*
void MovingObject::computeMetrics() {
    xn::SceneMetaData sceneMetaData;
    gen.user.GetUserPixels(id, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    XnPoint3D com;
    gen.user.GetCoM(id, com);
    XnPoint3D com2, com3;
    gen.depth.ConvertRealWorldToProjective(1, &com, &com2);
    gen.depth.ConvertProjectiveToRealWorld(1, &com, &com3);
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();
    if (com.Z != 0) {
        //printf("real world : (%f, %f, %f)\n", com.X, com.Y, com.Z);
        //printf("projective world : (%d, %d, %f)\n", (int)com2.X, (int)com2.Y, com2.Z);
        //printf("other world : (%d, %d, %f)\n", com3.X, com3.Y, com3.Z);
        int i = (int)com2.X;
        int j = (int)com2.Y;

        //computes the with of the object on the center of gravity
        XnPoint3D lcom;
        XnPoint3D rcom;
        int nbOther = 1;
        bool found = false;
        for (int y=j; y<XN_VGA_Y_RES; y++){
            for(int x=i;x<XN_VGA_X_RES;x--){
                if (nbOther==0){
                    lcom.X = x+1;
                    lcom.Y = y;
                    lcom.Z = pDepthMap[y * XN_VGA_X_RES + x + 1];
                    found = true;
                    break;
                }
                if (userPix[y * XN_VGA_X_RES + x ] != id) {
                    nbOther--;
                }
            }
            if (found) break;
        }
        nbOther = 1;
        found = false;
        for (int y=j; y<XN_VGA_Y_RES; y++){
            for(int x=i;x<XN_VGA_X_RES;x++){
                if (nbOther==0){
                    rcom.X = x-1;
                    rcom.Y = y;
                    rcom.Z = pDepthMap[y * XN_VGA_X_RES + x-1];
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
        //if(lcom.X > 0.1 && rcom.X > 0.1){
            printf("real world %d left : (%f, %f, %f)\n", id, lcom.X, lcom.Y, lcom.Z);
            printf("real world %d right : (%f, %f, %f)\n", id, rcom.X, rcom.Y, rcom.Z);
            float dist = getDistance(lcom, rcom);
            printf("real world %d distance : %f\n", id, dist);

        this->com.X = com2.X;
        this->com.Y = com2.Y;
        this->com.Z = com2.Z;

    }
}

float MovingObject::getHeight() {

    return height;
}
*/

void MovingObject::checkMovement() {
    int currentMovement = -1 ; // 0 : unknown   1 : going out   2 : entering
    float lastZ = 0;
    int startFrameCurrentMovement = frames[0].getId();
    int balanceCount=0;
    string typeMovement = "Unknown movement";
    bool newChange = false;
    bool firstChange = true;

    for (int i=0;i<frames.size();i++){
        float z = frames[i].getCom().Z; //get the current human depth
        if(z < 10000.0 && z > 0.1){   //possible value
            int move;
            //printf("\tcurrent move:%d newChange:%d\n", currentMovement, newChange);
            //printf("\tz:%f last:%f dif:%f\n", z, lastZ, (lastZ - z));
            if(lastZ - z > 0){  //Difference between 2 frames
                move = 2;
                if(currentMovement == move){
                    balanceCount++;
                }else{
                    balanceCount = 1;
                    typeMovement = "entering";
                }
            }else{
                move = 1;
                if(currentMovement == move){
                    balanceCount--;
                }else{
                    balanceCount = -1;
                    typeMovement = "going out";
                }
            }
            if ( abs(balanceCount)==3){ //if movement seems stable
                //printf("\tcurrent balanceCount:%d newChange:%d\n", balanceCount, newChange);
                if (firstChange){
                    firstChange = false;
                }else{
                    newChange = true;
                }
            }
            if(newChange || i==(frames.size()-1)){  //if changement of direction or end of frames
                //printf("new change. frames (%d to %d) lastz :%f (%f - %f)\n",startFrameCurrentMovement, frames[i].getId()-1,(lastZ- z), lastZ, z);
                //Add new event with information
                events.push_back(Event(startFrameCurrentMovement, frames[i].getId()-1, typeMovement.c_str()));
                startFrameCurrentMovement = frames[i].getId();
                newChange = false;
            }
            currentMovement = move; //update movement and current Z
            lastZ = z;
        }
    }
}

void MovingObject::computeDistance() {
    float distance = 0.0;
    XnPoint3D current;
    XnPoint3D last = frames[0].getCom();
    for (int i=1;i<frames.size();i++){
        current = frames[i].getCom();
        distance += getDistance(last, current);
        last = current;
    }

    metric.totDistance = distance;
}

float MovingObject::getDistance(XnPoint3D p1, XnPoint3D p2) {
    return sqrt(pow(p1.X-p2.X,2) + pow(p1.Y-p2.Y,2) + pow(p1.Z-p2.Z,2));
}

bool MovingObject::isValidWidthCountDown(){
    //printf("\tvalue:%d\n", validWidthCount);
    if(--validWidthCount < 0){
        validWidthCount = MAX_VALID_WIDTH_COUNT;
        return true;
    }

    return false;
}

void MovingObject::resetValidWidth(){
    metric.validWidth = 0;
    validWidthCount = MAX_VALID_WIDTH_COUNT;
}

void MovingObject::resetValidWidthCountDown(){
    validWidthCount = MAX_VALID_WIDTH_COUNT;
}

void MovingObject::toXML(TiXmlElement* sequenceNode) {
    XnUInt32 endFrameNo;
    gen.player.TellFrame(gen.depth.GetName(), endFrameNo);

    computeDistance();
    checkMovement();

    //printf("*** moving object xml %d***\n", id);

    TiXmlElement * movingObjectNode = new TiXmlElement("movingObject");
    movingObjectNode->SetAttribute("startFrameNo", startFrameNo);
    movingObjectNode->SetAttribute("endFrameNo", endFrameNo);
    movingObjectNode->SetAttribute("movingObjectType", "");
    movingObjectNode->SetAttribute("keyImage2d", file2d.c_str());
    movingObjectNode->SetAttribute("keyImage3d", file3d.c_str());
    sequenceNode->LinkEndChild(movingObjectNode);

    //output metric
    TiXmlElement * metricNode = new TiXmlElement("metric");
    metricNode->SetAttribute("height", metric.height);
    metricNode->SetAttribute("width", metric.width);
    metricNode->SetAttribute("totDistance", metric.totDistance);
    movingObjectNode->LinkEndChild(metricNode);

    //output events
    TiXmlElement * eventsNode = new TiXmlElement("events");
    for (int i=0;i<events.size();i++){
        events[i].toXML(eventsNode);
    }
    movingObjectNode->LinkEndChild(eventsNode);

    //output frames
    TiXmlElement * framesNode = new TiXmlElement("frames");
    for (int i=0;i<frames.size();i++){
        frames[i].toXML(framesNode);
    }
    movingObjectNode->LinkEndChild(framesNode);
}

void MovingObject::outputKeyImages() {
    XnUInt32 nFrame;
    gen.player.TellFrame(gen.depth.GetName(), nFrame);
    outputImage(frames[nFrame-startFrameNo].getZone(), file2d);
    outputDepth(frames[nFrame-startFrameNo].getZone(), file3d);
}

/*void MovingObject::outputImagesKey(std::ostringstream& file2d, std::ostringstream& file3d) {
    XnUInt32 no;


    int key = frames[frames.size()/2].getId();
    printf("key: %d\n", key);
    gen.player.SeekToFrame(gen.depth.GetName(), key, XN_PLAYER_SEEK_SET);
    gen.player.TellFrame(gen.depth.GetName(), no);
    printf("seek: %d\n", no);

    outputImage(frames[frames.size()/2].getZone(), file2d);

    //outputImage(frames[frames.size()/2].getZone());
    //outputDepth(rect);

    gen.player.SeekToFrame(gen.depth.GetName(), no, XN_PLAYER_SEEK_SET);
}
*/
