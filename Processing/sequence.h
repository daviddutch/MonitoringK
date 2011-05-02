#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <map>
#include "movingObject.h"
#include <qdom.h>
#include "defs.h"

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
    std::vector<MovingObject> movingObjects;
    XnPoint3D getComByUser(int id);
    bool isTwoPointClose(XnPoint3D p1, XnPoint3D p2);
    Metric computeMetrics(XnUserID userId, XnPoint3D com);
};

#endif // SEQUENCE_H
