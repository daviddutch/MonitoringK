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

class MovingObject
{
public:
    MovingObject();
    MovingObject(XnUserID pId, xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator);
    float getHeight();
    void update();
    void toXML();
    bool operator==(const MovingObject &movingObject) const; //equal
private:
    XnUserID id;
    float height;
    bool movingIn;
    bool movingOut;
    xn::UserGenerator userGenerator;
    xn::DepthGenerator depthGenerator;
    xn::ImageGenerator imageGenerator;
    XnPoint3D com; //Center of Mass
    int nFrame;
    int nEvents;
    //Frame frames[100];
    Event events[100];
    std::vector<Frame> frames;

    void outputImage(Rect rect);
    void outputDepth(Rect rect);
};

#endif // MOVINGOBJECT_H
