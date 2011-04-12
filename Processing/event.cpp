#include "event.h"
#include <stdio.h>

Event::Event()
{
}
void Event::init(std::string &type){
    this->type = type;
}
void Event::toXML() {
    printf("<event type=\"%s\" />", type.data());
}
