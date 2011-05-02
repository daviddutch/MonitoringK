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
    Sequence(Generators& gen, std::string dir);
    void update();
    void toXML(QDomDocument& doc, QDomElement& movieNode);

private:
    XnUInt32 startFrame;
    Generators& gen;
    std::string dir;
    std::map<int, MovingObject> movingObjects;
    XnPoint3D getComByUser(int id);
    bool isTwoPointClose(XnPoint3D p1, XnPoint3D p2);
};

#endif // SEQUENCE_H
