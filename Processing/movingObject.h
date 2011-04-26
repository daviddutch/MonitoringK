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
    MovingObject(XnUserID pId, Generators& generators, xn::Player& player);
    float getHeight();
    float getHeightByFrame(int i);
    std::string getTypeMovement();
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
    xn::Player& g_player;
    XnPoint3D com; //Center of Mass
    XnUInt32 startFrameNo;
    std::vector<Frame> frames;
    std::vector<Event> events;
    Metric metric;
    int nFrame;
    void outputImage(Rect rect);
    void outputDepth(Rect rect);
    void checkMovement(QDomDocument& doc, QDomElement& sequenceNode);
    void computeMetrics();
};

#endif // MOVINGOBJECT_H
