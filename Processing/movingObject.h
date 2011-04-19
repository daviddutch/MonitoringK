#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H
#include <stdio.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <vector>
#include "defs.h"
#include "frame.h"
#include "event.h"
#include "qdom.h"

class MovingObject
{
public:
    MovingObject();
    MovingObject(XnUserID pId, xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator, xn::Player& player);
    float getHeight();
    void update();
    void toXML(QDomDocument& doc, QDomElement& sequenceNode);
    bool operator==(const MovingObject &movingObject) const; //equal
private:
    XnUserID id;
    float height;
    bool movingIn;
    bool movingOut;
    xn::UserGenerator userGenerator;
    xn::DepthGenerator depthGenerator;
    xn::ImageGenerator imageGenerator;
    xn::Player g_player;
    XnPoint3D com; //Center of Mass
    int nFrame;
    XnUInt32 startFrameNo;
    int nEvents;
    //Frame frames[100];
    Event events[100];
    std::vector<Frame> frames;

    void outputImage(Rect rect);
    void outputDepth(Rect rect);
};

#endif // MOVINGOBJECT_H
