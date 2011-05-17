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

enum ObjectState { NEW, IN_SIGHT, OUT_OF_SIGHT, IN_BORDER, IN_SEPERATION, SEPERATED };


class MovingObject
{
public:
    MovingObject(XnUserID pId, Generators& generators, std::string dir);

    XnUserID getXnId();
    void setXnId(XnUserID xnUserId);

    Metric getMetric();

    XnPoint3D getCom();
    void update(Metric metric);
    void toXML(TiXmlElement* sequenceNode);
    bool operator==(const MovingObject &movingObject) const; //equal
    MovingObject& operator=(const MovingObject& rhs);  //assignement
    void outputKeyImages();

    bool isValidWidthCountDown();
    void resetValidWidthCountDown();
    void resetValidWidth();

    std::vector<Frame> getFrames();

private:
    static int next_id;
    int id;
    XnUserID xnUserId;
    std::string dir;
    Generators& gen;
    XnPoint3D com; //Center of Mass
    XnUInt32 startFrameNo;
    XnRGB24Pixel comColor;
    std::vector<Frame> frames;
    std::vector<Event> events;
    Metric metric;
    std::string file2d;
    std::string file3d;
    int stableHeight;
    int validWidthCount;

    StateVars stateVars;

    ObjectState state;

    void outputImage(Rect rect);
    void outputImage(Rect rect, std::string file);
    void outputDepth(Rect rect);
    void outputDepth(Rect rect, std::string file);
    void checkMovement();
    void computeDistance();
    float getDistance(XnPoint3D p1, XnPoint3D p2);
    void computeComColor();

    Metric computeMetrics(XnPoint3D com);

    float computeWidth();
    float computeHeight();

    void updateState();

    bool isInSeperation();

};

#endif // MOVINGOBJECT_H
