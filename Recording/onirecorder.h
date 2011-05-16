/**
 * @file onirecorder.h
 * @brief Header file of the OniRecorder class
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 * ONIRecorder
 */

#ifndef ONIRECORDER_H
#define ONIRECORDER_H

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <string>

/**
 * @class OniRecorder
 * @brief Class used to save an OpenNI flow
 *
 * Class used to save a video flow into a single file using OpenNI Framework
 * The output file contains the depth frames and the rgb frames
 * The file is in ONI format
 */
class OniRecorder
{
public:

    /**
     * @brief Constructor
     *
     * Constructor of the OniRecorder class
     *
     * @param The duration in seconds that the recording should last
     * @param The destination path where to save the ONI file
     */
    OniRecorder(long duration, std::string destination);


    /**
     * @brief Starts the recording
     */
    void start();


    /**
     * @brief Ends the recording
     */
    void stop();


private:
    xn::Context context;  /*!< OpenNI context*/
    xn::DepthGenerator depthGenerator;  /*!< The depth generator*/
    xn::ImageGenerator imageGenerator;  /*!< The image generator*/
    xn::Recorder* recorder;  /*!< The OpenNI recorder*/

    XnStatus nRetVal;  /*!< The return value*/

    std::string destination;  /*!< The destination path where to save the ONI file*/

    long duration;  /*!< Duration of the recording*/


    /**
     * @brief Initialize the OpenNI environment. The Kinect must be connected before this method is called
     *
     * Initialize the OpenNI environment. The Kinect must be connected before this method is called
     *
     */
    void init();


    /**
     * @brief The recording function called in the start function
     */
    void record();


    /**
     * @brief Ends the recording by shutting down the environment
     */
    void endRecording();
};

#endif // ONIRECORDER_H
