/**
 * @file frame.h
 * @brief Header file of the frame class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 */

#ifndef FRAME_H
#define FRAME_H
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "defs.h"
#include "tinyxml.h"

/**
 * @class Frame
 * @brief Class that describes a single frame
 *
 *  A frame describes one frame with important information's
 */
class Frame
{
public:
    /**
     * @brief Constructor
     *
     * Constructor of the Frame class
     *
     * @param The frame number
     * @param The interesting zone containing the moving object
     * @param The center of mass coordinate of the moving object on the frame (real world)
     */
    Frame(int frame, Rect zone, XnPoint3D com);


    /**
     * @brief Destructor
     *
     * Destructor of the Event class
     */
    ~Frame();


    /**
     * @brief Adds the class content in the xml node
     *
     * Adds the class content in the xml node. The node should be of frames type
     *
     * @param The frames node
     */
    void toXML(TiXmlElement* framesNode);


    /**
     * @brief getter for the center of mass
     *
     * @return the center of mass
     */
    XnPoint3D getCom();


    /**
     * @brief getter for the zone attribute
     *
     * @return the rect zone containing the moving object
     */
    Rect getZone();


    /**
     * @brief Getter for the frame number
     *
     * @return the frame number
     */
    int getId();


private:
    int frame;     /*!< The frame number*/
    Rect zone;     /*!< The interesting zone containing the moving object*/
    XnPoint3D com; /*!< The center of mass coordinate of the moving object on the frame (real world)*/
};

#endif // FRAME_H
