#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H
#include <stdio.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <vector>
#include "defs.h"
#include "generators.h"
#include "frame.h"
#include "event.h"
#include "qdom.h"
#include <iostream>
#include <string>

class MovingObject
{
public:
    MovingObject(XnUserID pId, Generators& generators);
    float getHeight();
    float getHeightByFrame(int i);
    XnPoint3D getCom();
    int getId();
    void update();
    void toXML(QDomDocument& doc, QDomElement& sequenceNode);
    bool operator==(const MovingObject &movingObject) const; //equal
    //MovingObject& operator=(const Date& rhs);  //assignement
private:
    XnUserID id;
    float height;
    bool movingIn;
    bool movingOut;
    Generators& gen;
    XnPoint3D com; //Center of Mass
    XnUInt32 startFrameNo;
    std::vector<Frame> frames;
    std::vector<Event> events;
    Metric metric;
    int nFrame;
    void outputImage(Rect rect);
    void outputDepth(Rect rect);
    void checkMovement(QDomDocument& doc, QDomElement& sequenceNode);
    float checkDistance();
    float getDistance(XnPoint3D p1, XnPoint3D p2);
    void computeMetrics();
    void outputImagesKey();
};

#endif // MOVINGOBJECT_H
