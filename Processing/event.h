#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "tinyxml.h"

class Event
{
public:
    Event(int startFrameNo, int endFrameNo, std::string type);
    void toXML(TiXmlElement* eventsNode);
private:
    int startFrameNo;
    int endFrameNo;
    std::string type;
};

#endif // EVENT_H
