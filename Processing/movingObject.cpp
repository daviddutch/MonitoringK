#include "movingObject.h"
#include <iostream>
#include <string>
#include <cv.h>
#include <cxcore.h>
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
    std::ostringstream file1, file2;
    file1 << dir << "/2D/keyimage-" << id << ".png";
    file2 << dir << "/3D/keyimage-" << id << ".png";
    file2d = file1.str();
    file3d = file2.str();
    gen.player.TellFrame(gen.image.GetName(), startFrameNo);
    stableHeight = 5;
    stateVars.inSeperationCount = MAX_VALID_WIDTH_COUNT;
    state = NEW;
    metric.height = 0;
    metric.width = 0;
    metric.validWidth = 0;
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
ObjectState MovingObject::getState(){
    return state;
}
void MovingObject::setState(ObjectState s){
    state=s;
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

    cv::imwrite(file, rgbimg);

    chmod(file.c_str(), 0777);

    cvReleaseImageHeader(&rgbimg);
    //delete[] ucpImage;
}

void MovingObject::outputDepth(Rect rect, std::string file) {
    double alpha = 255.0/2048.0;    //gray alpha
    //compute the inside rect to draw rectangle
    Rect rect2;
    rect2.top    = rect.top+2;
    rect2.right  = rect.right-2;
    rect2.bottom = rect.bottom-2;
    rect2.left   = rect.left+2;

    //Get pointer on depthMap and prepare matrix to get the cut depth image
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();
    CvMat* depthMetersMat   = cvCreateMat(480, 640, CV_8UC1);

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
    XnUInt32 nFrame2D;
    gen.player.TellFrame(gen.image.GetName(), nFrame2D);

    xn::SceneMetaData sceneMetaData;
    xn::DepthMetaData depthMetaData;

    gen.depth.GetMetaData(depthMetaData);

    gen.user.GetUserPixels(id, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    gen.user.GetCoM(xnUserId, com);


    if (com.Z > 0.001){ //is that correct, working
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
        frames.push_back(Frame(nFrame2D, rect, com));

        //computeComColor();

        float evolvHeight = abs(metric.height-this->metric.height)/metric.height;
        if (evolvHeight<0.1 && stableHeight>0){
            stableHeight--;
            if (stableHeight==0){
                outputKeyImages();
                metric.validWidth = metric.width;
            }
        }else if (stableHeight>0){
            stableHeight=5;
        }

        //metric = newMetric;
        if (nFrame2D==startFrameNo && state != NEW && state != OUT_OF_SIGHT)
            outputKeyImages();
    }

    updateState();
}
void MovingObject::updateState() {
    if (state == SEPERATED) return;

    if ((com.Z < 0.001) && state == NEW){ //stay NEW
        return;
    }
    if ((com.Z < 0.001) && state != NEW){ //anyway != then NEW
        state = OUT_OF_SIGHT;
        xnUserId = 0;
        return;
    }

    if (frames.size()!=0) {
        Rect rect = frames[frames.size()-1].getZone();
        if (rect.top<5 || rect.left<10 || rect.bottom==(XN_VGA_Y_RES-5) || rect.right==(XN_VGA_X_RES-10) ){
            state = IN_BORDER;
            return;
        }
    }
    bool isInSep = isInSeperation();
    if (isInSep && state != IN_SEPERATION) {
        state = IN_SEPERATION;
        stateVars.inSeperationCount = MAX_VALID_WIDTH_COUNT;
        return;
    }else if (isInSep){
        stateVars.inSeperationCount--;
        if (stateVars.inSeperationCount==0){
            state = SEPERATED;
        }
        return;
    }else{
        metric.validWidth = metric.width;
    }

    state = IN_SIGHT;
}
bool MovingObject::isInSeperation() {
    Metric newMetric;
    newMetric.height = computeHeight();
    newMetric.width  = computeWidth();

    //Metric oldMetric = movingObjects[indexUser].getMetric();
    if(newMetric.height==0.0 || newMetric.width==0.0)
        return false;

    if (newMetric.width<0){
        newMetric.height = metric.height;
        newMetric.width = metric.width;
    }

    float evolvWidth = abs(newMetric.width-metric.validWidth)/newMetric.width;
    //printf(" evolv width %f / (old:%f, new:%f, valid:%f)\n", evolvWidth , metric.width, newMetric.width, metric.validWidth);

    metric.height = newMetric.height;
    metric.width = newMetric.width;

    if (evolvWidth>0.6){
        return true;
    }
    return false;
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

    if (com.Z > 0.001) {
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
    }
}


std::vector<Frame> MovingObject::getFrames(){
    return frames;
}

void MovingObject::checkMovementSimple(){
    float zStart = frames[0].getCom().Z;
    float zEnd = frames[frames.size()-1].getCom().Z;
    string event = "Unknown movement";
    if(zStart > zEnd){
        event = "Entering";
    }else{
        event = "Going out";
    }
    events.push_back(Event(frames[0].getId(), frames[frames.size()-1].getId(), event.c_str()));
}
//TODO : Fix bugs
void MovingObject::checkMovement() {
    int currentMovement = -1 ; // 0 : unknown   1 : going out   2 : entering
    int tempMovement = -1;
    float lastZ = 0;
    int startFrameCurrentMovement = frames[0].getId();
    int balanceCount=0;
    string typeMovement = "Unknown movement";
    bool newChange = false;
    bool firstChange = true;

    for (int i=0;i<frames.size();i++){
        float z = frames[i].getCom().Z; //get the current human depth
        //printf("%d z:%f\n",id, z);
        if(z < 10000.0 && z > 0.1){   //possible value
            int move;
            if(lastZ >= z){  //Difference between 2 frames
                move = 2;
                if(currentMovement == move){
                    //Nothing to do
                }else if(tempMovement == move){
                    balanceCount++;
                }else{
                    balanceCount = 1;

                }
            }else{
                move = 1;
                if(currentMovement == move){
                    //Nothing to do
                }else if(tempMovement == move){
                    balanceCount--;
                }else{
                    balanceCount = -1;

                }
            }
            if ( abs(balanceCount)==10 && currentMovement != move){ //if movement seems stable
                if (firstChange){
                    firstChange = false;
                }else{
                    newChange = true;
                }
                currentMovement = move;
                typeMovement = tempMovement==2 ? "going out" :"entering" ;
                //printf("new movement stable: %s\n", typeMovement.c_str());
            }
            if(newChange || i==(frames.size()-1)){  //if changement of direction or end of frames
                //Add new event with information
                //printf("new event %d: %d,%d,%s\n",id, startFrameCurrentMovement, frames[i].getId()-1, typeMovement.c_str());
                events.push_back(Event(startFrameCurrentMovement, frames[i].getId()-1, typeMovement.c_str()));
                startFrameCurrentMovement = frames[i].getId();
                newChange = false;
            }
            tempMovement = move; //update movement and current Z
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
    //sqrt[(Xa-Xb)²+(Ya-Yb)²+(Za-Zb)²]
    return sqrt(pow(p1.X-p2.X,2) + pow(p1.Y-p2.Y,2) + pow(p1.Z-p2.Z,2));
}

void MovingObject::toXML(TiXmlElement* sequenceNode) {
    if(frames.size()<=0)
        return;

    computeDistance();
    checkMovementSimple();

    int endFrameNo;
    endFrameNo = frames[frames.size()-1].getId();

    TiXmlElement * movingObjectNode = new TiXmlElement("movingObject");
    movingObjectNode->SetAttribute("id", id);
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
    gen.player.TellFrame(gen.image.GetName(), nFrame);
    Frame f = findFrameById(nFrame);
    Rect r = f.getZone();
    outputImage(r, file2d);
    outputDepth(r, file3d);
}

float MovingObject::computeWidth() {
    xn::SceneMetaData sceneMetaData;
    gen.user.GetUserPixels(xnUserId, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    XnPoint3D com2;
    gen.depth.ConvertRealWorldToProjective(1, &com, &com2);
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();

    int i = (int)com2.X;
    int j = (int)com2.Y;

    //----computes the with of the object on the center of gravity
    XnPoint3D lcom;
    XnPoint3D rcom;
    int nbOther = 1;

    float f = pDepthMap[j * XN_VGA_X_RES + i];
    if(f<0.1){ // wrong data
        return -1;
    }
    lcom.X = 0;
    lcom.Y = j;
    lcom.Z = com.Z;
    for(int x=i;x<XN_VGA_X_RES;x--){
        if (nbOther==0){
            lcom.X = x+2;
            float f = pDepthMap[j * XN_VGA_X_RES + x+2];
            lcom.Z = f;
            break;
         }
        if (userPix[j * XN_VGA_X_RES + x ] != xnUserId) {
            nbOther--;
         }
    }
    nbOther = 1;
    rcom.X = XN_VGA_X_RES-1;
    rcom.Y = j;
    rcom.Z = com.Z;
    for(int x=i;x<XN_VGA_X_RES;x++){
        if (nbOther==0){
            rcom.X = x-2;
            float f = pDepthMap[j * XN_VGA_X_RES + x-2];
            rcom.Z = f;
            break;
        }
        if (userPix[j * XN_VGA_X_RES + x ] != xnUserId) {
            nbOther--;
        }
    }

    if(lcom.X<=20 || rcom.X>=(XN_VGA_X_RES-20) ){   //Border case
        return -1;
    }

    XnPoint3D lcom2;
    XnPoint3D rcom2;

    gen.depth.ConvertProjectiveToRealWorld(1, &lcom, &lcom2);
    gen.depth.ConvertProjectiveToRealWorld(1, &rcom, &rcom2);

    if(lcom2.Z > 0.1 && rcom2.Z > 0.1){
        return rcom2.X - lcom2.X;
    }else{
        printf("ERROR METRIC\n");
        printf("%d real world right : (%f, %f, %f)\n", xnUserId, rcom.X, rcom.Y, rcom.Z);
        printf("%d real world 2 right : (%f, %f, %f)\n", xnUserId, rcom2.X, rcom2.Y, rcom2.Z);
        printf("%d real world left : (%f, %f, %f)\n", xnUserId, lcom.X, lcom.Y, lcom.Z);
        printf("%d real world 2 left : (%f, %f, %f)\n", xnUserId, lcom2.X, lcom2.Y, lcom2.Z);
        printf("%d com  world : (%f, %f, %f)\n", xnUserId, com.X, com.Y, com.Z);
        return -1;
    }
}
float MovingObject::computeHeight() {
    xn::SceneMetaData sceneMetaData;
    gen.user.GetUserPixels(xnUserId, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    XnPoint3D com2;
    gen.depth.ConvertRealWorldToProjective(1, &com, &com2);
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();

    int i = (int)com2.X;

    //----computes the height of the object
    XnPoint3D top;
    top.X = i;
    top.Y = 0;
    top.Z = com.Z;
    XnPoint3D bottom;
    bottom.X = i;
    bottom.Y = XN_VGA_Y_RES-1;
    bottom.Z = com.Z;
    //top
    for (int y=0; y<XN_VGA_Y_RES; y++){
        for(int x=0;x<XN_VGA_X_RES;x++){
            if (userPix[y * XN_VGA_X_RES + x ] == xnUserId) {
                top.X = x;
                top.Y = y;
                top.Z = pDepthMap[y * XN_VGA_X_RES + x];
                y = XN_VGA_Y_RES; //make it exit the outer loop
                break;
            }
        }
    }
    //bottom
    for (int y=XN_VGA_Y_RES-1; y>=0; y--){
        for(int x=XN_VGA_X_RES-1;x>=0;x--){
            if (userPix[y * XN_VGA_X_RES + x ] == xnUserId) {
                bottom.X = x;
                bottom.Y = y;
                bottom.Z = pDepthMap[y * XN_VGA_X_RES + x];
                y = -1; //make it exit the outer loop
                break;
            }
         }
     }

    if(top.X<=10 || bottom.X>=(XN_VGA_Y_RES-10) ){  //Border case
        return -1;
    }

    gen.depth.ConvertProjectiveToRealWorld(1, &top, &top);
    gen.depth.ConvertProjectiveToRealWorld(1, &bottom, &bottom);

    return top.Y-bottom.Y;
}

Metric MovingObject::computeMetrics(XnPoint3D com) {
    Metric metric;

    return metric;
}

Frame MovingObject::findFrameById(int id){
    Frame* f = 0;
    for (int i=0;i<frames.size();i++){
        if(frames[i].getId() == id)
            return frames[i];
    }
    return *f;
}


