#include "frame.h"
#include "qdom.h"

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

void Frame::toXML(QDomDocument& doc, QDomElement& framesNode) {
    QDomElement frameNode = doc.createElement("frame");
    framesNode.appendChild(frameNode);

    QDomElement rectZoneNode = doc.createElement("rectZone");
    rectZoneNode.setAttribute("top",zone.top);
    rectZoneNode.setAttribute("right",zone.right);
    rectZoneNode.setAttribute("bottom",zone.bottom);
    rectZoneNode.setAttribute("left",zone.left);
    frameNode.appendChild(rectZoneNode);

    QDomElement comNode = doc.createElement("com");
    comNode.setAttribute("x",com.X);
    comNode.setAttribute("y",com.Y);
    comNode.setAttribute("z",com.Z);
    frameNode.appendChild(comNode);
}
Frame::~Frame() {
    //delete zone;
    //delete com;
}
