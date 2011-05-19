/**
 * @file sequence.h
 * @brief Header file of the Sequence class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 */
#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <map>
#include "movingObject.h"
#include "defs.h"

/**
 * @class Sequence
 * @brief Class that describe Sequence
 *
 *  A sequence is a part of movie where there is one or more user inside
 */
class Sequence
{
public:
    /**
     * @brief Constructor
     *
     * Constructor a new sequence
     *
     * @param generators
     * @param directory to save data
     */
    Sequence(Generators& gen, std::string dir);

    /**
     * @brief update sequence
     *
     * Called each frame
     *
     */
    void update();

    /**
     * @brief Adds the class content in the xml node
     *
     * Adds the class content in the xml node. The node should be of movieNode type
     *
     * @param The movieNode node
     */
    void toXML(TiXmlElement* movieNode);

private:
    XnUInt32 startFrame;                        /*!< No start frame */
    Generators& gen;                            /*!< Generators */
    std::string dir;                            /*!< directory to save data */
    std::vector<MovingObject> movingObjects;    /*!< list of movingObjects */

};

#endif // SEQUENCE_H
