#include "event.h"
#include <stdio.h>


Event::Event(int startFrameNo, int endFrameNo, std::string type)
{
    this->startFrameNo = startFrameNo;
    this->endFrameNo = endFrameNo;
    this->type = type;
}
void Event::toXML(TiXmlElement* eventsNode) {
    TiXmlElement * eventNode = new TiXmlElement("event");
    eventNode->SetAttribute("startFrameNo", startFrameNo);
    eventNode->SetAttribute("endFrameNo", endFrameNo);
    eventNode->SetAttribute("typeEvent", type.c_str());
    eventsNode->LinkEndChild(eventNode);
}
