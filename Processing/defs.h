/**
 * @file defs.h
 * @brief General definition file
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 * General definition file with types and constants that a global to the whole program
 */

#ifndef DEFS_H
#define DEFS_H

#include <XnOpenNI.h>

#define SAMPLE_XML_PATH "Sample-User.xml"

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {		 														\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480

#define MAX_VALID_WIDTH_COUNT 5


/**
 * @struct Rect
 * @brief Describes a rectangle with 4 integers
 *
 * Describes a rectangle with 4 integers values (top, right, bottom, left)
 */
struct Rect {
    int top;    /*!< Top value */
    int right;  /*!< Right value */
    int bottom; /*!< Bottom value */
    int left;   /*!< Left value */
};

/**
 * @struct Rect3D
 * @brief Describes a rectangle with 3D points
 *
 * Describes a rectangle with 4 3D points top, right, bottom, left)
 */
struct Rect3D {
    XnPoint3D top;    /*!< Top value */
    XnPoint3D right;  /*!< Right value */
    XnPoint3D bottom; /*!< Bottom value */
    XnPoint3D left;   /*!< Left value */
};

/**
 * @struct Metric
 * @brief Describes the metric values of an moving object
 *
 * Contains size informations, distance values, ...
 */
struct Metric {
    float height;     /*!< Height of the object */
    float width;      /*!< Width of the object */
    float validWidth; /*!< A valid width */
    float totDistance;/*!< The total distance the object moved */
};

/**
 * @struct StateVars
 * @brief Variables describing a state
 *
 * Variables describing a state
 */
struct StateVars {
    int inSeperationCount;
    int seperatedCount;

};

/**
 * @enum ObjectState
 * @brief variable describing moving object state
 *
 * variable describing moving object state:
 * NEW, IN_SIGHT, OUT_OF_SIGHT, IN_BORDER, IN_SEPERATION, SEPERATED
 */
enum ObjectState { NEW, IN_SIGHT, OUT_OF_SIGHT, IN_BORDER, IN_SEPERATION, SEPERATED };
#endif // DEFS_H
