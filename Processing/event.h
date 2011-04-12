#ifndef EVENT_H
#define EVENT_H

#include <string>

class Event
{
public:
    Event();
    void init(std::string &type);
    void toXML();
private:
    std::string type;
};

#endif // EVENT_H
