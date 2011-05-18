/**
 * @file processor.h
 * @brief Header file of the Processor class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 */
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <vector>
#include "defs.h"
#include "generators.h"
#include "sequence.h"
#include <iostream>
#include <string>
#include "tinyxml.h"

/**
 * @class Processor
 * @brief Class that describe Processor
 *
 *  A Process is the main class of Processing. It manage the callbacks function and interaction with OpenNI
 */
class Processor
{
public:
    /**
     * @brief start processing
     *
     * start processing and init all Generators, XML, callbacks function, display.
     *
     * @return program statut
     */
    int start(int argc, char **argv);

    /**
     * @brief get unique instance of Processor
     *
     * get unique instance of Processor.
     *
     * @return instance
     */
    static Processor *getInstance();

    /**
     * @brief a new User is detected (Callback function)
     *
     * a new User is detected (Callback function). OpenNI
     *
     */
    static void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie);

    /**
     * @brief a User is lost (Callback function)
     *
     * a new User is lost (Callback function). OpenNI
     *
     */
    static void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie);
private:

    /**
     * @brief Constructor
     *
     * private Constructor of the MovingObject class
     *
     * @param user id of OpenNI
     * @param contains all generators
     * @param directory to save key images
     */
    Processor();

    static Processor *instance; /*!< unique instance of Processor */
    Generators* gen;            /*!< Generators OpenNI */
    xn::Context context;        /*!< Context OpenNI */
    int nUser;                  /*!< number of user */
    bool hasUserInSight;        /*!< is someone in sight */
    std::string dir;            /*!< current directory to save key images */
    Sequence* sequence;         /*!< sequence of processing */
    TiXmlDocument doc;          /*!< XML document */
    TiXmlElement* movieNode;    /*!< XML root element */
    std::string dateStart;      /*!< Start date of movie */
    std::string fileName;       /*!< fileName ONI */
    const XnChar* strNodeName;  /*!< Node name OpenNI */

    /**
     * @brief create a XML document
     *
     * create a XML document
     *
     */
    void createXML();

    /**
     * @brief save XML to file
     *
     * save XML to file
     *
     */
    void writeXML();

    /**
     * @brief Clean context OpenNI
     *
     * Clean context OpenNI
     *
     */
    static void CleanupExit();

    /**
     * @brief prepare data to display to screen. (Glut)
     *
     * this function is called each frame
     *
     */
    static void glutDisplay (void);

    /**
     * @brief Glut idle function
     *
     * Glut idle function
     *
     */
    static void glutIdle (void);

    /**
     * @brief watch if user press a key (Glut)
     *
     * this function is called each frame
     *
     */
    static void glutKeyboard (unsigned char key, int x, int y);

    /**
     * @brief init function Glut
     *
     * init function Glut
     *
     */
    void glInit (int * pargc, char ** argv);

    /**
     * @brief play next frame
     *
     * play next frame using Player generator
     *
     */
    static void playNextFrame();

    /**
     * @brief play previous frame
     *
     * play previous frame using Player generator
     *
     */
    static void playPrevFrame();


};

static bool g_bStep;    /*!< is player has need next step */
static bool g_bPause;   /*!< is player is on pause */
#endif // PROCESSOR_H
