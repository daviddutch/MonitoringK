#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <map>
#include "movingObject.h"

class Sequence
{
public:
    Sequence(int startFrame, xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator);
    void update();
    void toXML();

private:
    int startFrame;
    xn::UserGenerator userGenerator;
    xn::DepthGenerator depthGenerator;
    xn::ImageGenerator imageGenerator;

    //MovingObject movingObjects[3];
    std::map<int, MovingObject> movingObjects;

    int nObjects;
};

#endif // SEQUENCE_H
