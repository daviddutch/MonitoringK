/**
 * @file generators.h
 * @brief Header file of the Generators class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 */
#ifndef GENERATORS_H
#define GENERATORS_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

/**
 * @class Generators
 * @brief Class that contains the main OpenNI generators
 *
 *  The class contains a reference to each generators needed during the processing
 */
class Generators
{
public:
    /**
     * @brief Constructor
     *
     * Constructor of the Generators class
     *
     * @param The user generator
     * @param The depth generator
     * @param The image generator
     * @param The player generator
     */
    Generators(xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator, xn::Player& pGenerator);


    xn::UserGenerator& user;   /*!< Reference to the user generator*/
    xn::DepthGenerator& depth; /*!< Reference to the depht generator*/
    xn::ImageGenerator& image; /*!< Reference to the image generator*/
    xn::Player& player;        /*!< Reference to the player generator*/
};

#endif // GENERATORS_H
