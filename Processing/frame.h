#ifndef FRAME_H
#define FRAME_H
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "defs.h"

class Frame
{
public:
    Frame(int frame, Rect &zone, XnPoint3D &com);
    void toXML();
private:
    int frame;
    Rect zone;
    XnPoint3D com; //Center of Mass
};

#endif // FRAME_H
