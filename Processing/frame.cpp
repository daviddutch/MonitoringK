#include "frame.h"

Frame::Frame(int frame, Rect &zone, XnPoint3D &com)
{
    this->frame = frame;
    this->zone  = zone;
    this->com   = com;
}
void Frame::toXML() {
    printf("\t\t<frame>\n");
    printf("\t\t\t<zone t= r= b= l=>\n");
    printf("\t\t\t<com x= y= z=>\n");
    printf("\t\t</frame>\n");
}
