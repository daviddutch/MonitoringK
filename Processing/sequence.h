#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <map>
#include "movingObject.h"
#include <qdom.h>

class Sequence
{
public:
    Sequence(xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator, xn::Player& g_player);
    void update();
    void toXML(QDomDocument& doc, QDomElement& movieNode);

private:
    XnUInt32 startFrame;
    xn::UserGenerator userGenerator;
    xn::DepthGenerator depthGenerator;
    xn::ImageGenerator imageGenerator;
    xn::Player g_player;

    //MovingObject movingObjects[3];
    std::map<int, MovingObject> movingObjects;

    int nObjects;
};

#endif // SEQUENCE_H
