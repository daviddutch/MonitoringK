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
    Sequence(Generators& gen, xn::Player& g_player);
    void update();
    void toXML(QDomDocument& doc, QDomElement& movieNode);

private:
    XnUInt32 startFrame;
    xn::Player& g_player;
    Generators& gen;
    std::map<int, MovingObject> movingObjects;
};

#endif // SEQUENCE_H
