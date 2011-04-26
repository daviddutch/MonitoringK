#ifndef FRAME_H
#define FRAME_H
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "defs.h"
#include "qdom.h"

class Frame
{
public:
    Frame(int frame, Rect &zone, XnPoint3D &com);
    void toXML(QDomDocument& doc, QDomElement& framesNode);
    XnPoint3D getCom();
    Rect getZone();
    int getId();
private:
    int frame;
    Rect zone;
    XnPoint3D com; //Center of Mass
};

#endif // FRAME_H
