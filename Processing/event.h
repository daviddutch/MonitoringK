/**
 * @file event.h
 * @brief Header file of the event class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 */

#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "tinyxml.h"

/**
 * @class Event
 * @brief Class that describes an event
 *
 *  An event is a action made by a moving object (enter the room for example)
 */
class Event
{
public:
    /**
     * @brief Constructor
     *
     * Constructor of the Event class
     *
     * @param The frame number of when the event starts
     * @param The frame number of when the event finishes
     * @param The event type/description
     */
    Event(int startFrameNo, int endFrameNo, std::string type);

    /**
     * @brief Destructor
     *
     * Destructor of the Event class
     */
    ~Event();


    /**
     * @brief Adds the class content in the xml node
     *
     * Adds the class content in the xml node. The node should be of events type
     *
     * @param The events node
     */
    void toXML(TiXmlElement* eventsNode);


private:
    int startFrameNo;  /*!< The frame number of when the event starts*/
    int endFrameNo;    /*!< The frame number of when the event finishes*/
    std::string type;  /*!< The event type/description*/
};

#endif // EVENT_H
