/**
 * @file movingObject.h
 * @brief Header file of the MovingObject class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 */

#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H
#include <stdio.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <vector>
#include "defs.h"
#include "generators.h"
#include "frame.h"
#include "event.h"
#include "tinyxml.h"
#include <iostream>
#include <string>


/**
 * @class MovingObject
 * @brief Class that describe a single movingObject
 *
 *  A MovingObject is a USER detected by OpenNI
 */
class MovingObject
{
public:
    /**
     * @brief Constructor
     *
     * Constructor of the MovingObject class
     *
     * @param user id of OpenNI
     * @param contains all generators
     * @param directory to save key images
     */
    MovingObject(XnUserID pId, Generators& generators, std::string dir);

    /**
     * @brief get user id supply by OpenNI
     *
     * get user id supply by OpenNI
     *
     * @return user id
     */
    XnUserID getXnId();

    /**
     * @brief set user id of OpenNI
     *
     * set user id of OpenNI
     *
     * @param user id
     */
    void setXnId(XnUserID xnUserId);

    /**
     * @brief get metric data
     *
     * get metric data
     *
     * @return metric data
     */
    Metric getMetric();

    /**
     * @brief get COM
     *
     * get COM. COM is a center point3D of mass of user
     *
     * @return Point3D COM
     */
    XnPoint3D getCom();

    /**
     * @brief update MovingObject with new metric
     *
     * update MovingObject with new metric
     *
     * @param new metric
     */
    void update(Metric metric);

    /**
     * @brief Adds the class content in the xml node
     *
     * Adds the class content in the xml node. The node should be of sequence type
     *
     * @param The sequence node
     */
    void toXML(TiXmlElement* sequenceNode);

    /**
     * @brief is MovingObject is the same
     *
     * is MovingObject is the same
     *
     * @param MovingObject to compare
     * @return is MovingObject is the same
     */
    bool operator==(const MovingObject &movingObject) const; //equal

    /**
     * @brief default assignement operator
     *
     * default assignement operator
     *
     * @param an OTHER MovingObject
     * @return new MovingObject
     */
    MovingObject& operator=(const MovingObject& rhs);  //assignement

    /**
     * @brief get current state
     *
     * get current state
     *
     * @return current state
     */
    ObjectState getState();

    /**
     * @brief set current state
     *
     * set current state
     *
     * @param new state
     */
    void setState(ObjectState s);

    /**
     * @brief get all frames
     *
     * get all frames
     *
     * @return all frames
     */
    std::vector<Frame> getFrames();

private:
    static int next_id;         /*!< user id available (global) */
    int id;                     /*!< current user id */
    XnUserID xnUserId;          /*!< user id by OpenNI */
    std::string dir;            /*!< directory to save key images */
    Generators& gen;            /*!< All Generators */
    XnPoint3D com;              /*!< Center of Mass */
    XnUInt32 startFrameNo;      /*!< No start frame */
    XnRGB24Pixel comColor;      /*!< Color of COM */
    std::vector<Frame> frames;  /*!< Frames of MovingObject */
    std::vector<Event> events;  /*!< Events of MovingObject*/
    Metric metric;              /*!< Metric data */
    std::string file2d;         /*!< path to save key image RGB */
    std::string file3d;         /*!< path to save key image Depth */
    int stableHeight;           /*!< stable value of Height */
    StateVars stateVars;        /*!< Statistic variables */
    ObjectState state;          /*!< current state of MovingObject */

    /**
     * @brief save key images
     *
     * save key images
     *
     */
    void outputKeyImages();

    /**
     * @brief save key images RGB
     *
     * save key images RGB
     *
     * @param Rectangle around the MovingObject
     * @param path file
     */
    void outputImage(Rect rect, std::string file);

    /**
     * @brief save key images Depth
     *
     * save key images Depth
     *
     * @param Rectangle around the MovingObject
     * @param path file
     */
    void outputDepth(Rect rect, std::string file);

    /**
     * @brief Compute Events of MovingObject (Not working..)
     *
     * Compute Events of MovingObject (Not working..)
     *
     */
    void checkMovement();

    /**
     * @brief Compute Events of MovingObject
     *
     * Compute Events of MovingObject
     *
     */
    void checkMovementSimple();

    /**
     * @brief Compute totale distance of MovingObject
     *
     * Compute totale distance of MovingObject
     *
     */
    void computeDistance();

    /**
     * @brief get distance of 2 Point3D
     *
     * get distance of 2 Point3D
     *
     * @param first point3D
     * @param second point3D
     */
    float getDistance(XnPoint3D p1, XnPoint3D p2);

    /**
     * @brief Compute color of COM (NOT USED YET)
     *
     * Compute color of COM (NOT USED YET)
     *
     */
    void computeComColor();

    /**
     * @brief get new metric with COM point
     *
     * get new metric with COM point
     *
     * @param COM point
     */
    Metric computeMetrics(XnPoint3D com);

    /**
     * @brief get MovingObject width
     *
     * get MovingObject width
     *
     */
    float computeWidth();

    /**
     * @brief get MovingObject height
     *
     * get MovingObject height
     *
     */
    float computeHeight();

    /**
     * @brief update current state
     *
     * update current state
     *
     */
    void updateState();

    /**
     * @brief is MovingObject has 2 people
     *
     * is MovingObject has 2 people. Processing with height and widht and compare with last values
     *
     */
    bool isInSeperation();

    /**
     * @brief get the frame by id frame
     *
     * get the frame by id frame
     *
     */
    Frame findFrameById(int id);
};

#endif // MOVINGOBJECT_H
