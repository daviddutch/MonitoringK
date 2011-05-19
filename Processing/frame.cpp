#include "frame.h"

Frame::Frame(int frame, Rect zone, XnPoint3D com)
{
    this->frame = frame;
    this->zone  = zone;
    this->com   = com;
}

XnPoint3D Frame::getCom()
{
    return com;
}

Rect Frame::getZone()
{
    return zone;
}

int Frame::getId()
{
    return frame;
}

void Frame::toXML(TiXmlElement* framesNode) {
    TiXmlElement * frameNode = new TiXmlElement("frame");
    frameNode->SetAttribute("id", frame);
    framesNode->LinkEndChild(frameNode);


    TiXmlElement * rectZoneNode = new TiXmlElement("rectZone");
    rectZoneNode->SetAttribute("top", zone.top);
    rectZoneNode->SetAttribute("right", zone.right);
    rectZoneNode->SetAttribute("bottom", zone.bottom);
    rectZoneNode->SetAttribute("left", zone.left);
    frameNode->LinkEndChild(rectZoneNode);

    TiXmlElement * comNode = new TiXmlElement("com");
    comNode->SetAttribute("x", com.X);
    comNode->SetAttribute("y", com.Y);
    comNode->SetAttribute("z", com.Z);
    frameNode->LinkEndChild(comNode);
}
Frame::~Frame() {
    //delete zone;
    //delete com;
}
