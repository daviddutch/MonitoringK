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
    MovingObject(XnUserID pId, Generators& generators, std::string dir);

    XnUserID getXnId();
    void setXnId(XnUserID xnUserId);

    Metric getMetric();
    void setMetric(Metric metric);

    XnPoint3D getCom();
    void update();
    void toXML(QDomDocument& doc, QDomElement& sequenceNode);
    bool operator==(const MovingObject &movingObject) const; //equal
    //MovingObject& operator=(const Date& rhs);  //assignement
private:
    static int next_id;
    int id;
    XnUserID xnUserId;
    std::string dir;
    Generators& gen;
    XnPoint3D com; //Center of Mass
    XnUInt32 startFrameNo;
    XnRGB24Pixel comColor;
    std::vector<Frame> frames;
    std::vector<Event> events;
    Metric metric;

    void outputImage(Rect rect);
    void outputImage(Rect rect, std::ostringstream& file);
    void outputDepth(Rect rect);
    void outputDepth(Rect rect, std::ostringstream& file);
    void checkMovement(QDomDocument& doc, QDomElement& sequenceNode);
    float checkDistance();
    float getDistance(XnPoint3D p1, XnPoint3D p2);
    void computeComColor();
    void outputImagesKey(std::ostringstream& file2d, std::ostringstream& file3d);
};

#endif // MOVINGOBJECT_H
