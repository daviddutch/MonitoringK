/**
 * @file main.cpp
 * @brief Entrance monitoring with a Kinect
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 * Main starting point of the program
 */

#include "processor.h"


/**
 * @brief Main starting point of the program
 *
 * @param Argument count
 * @param Arguments values
 *
 * @return 0
*/
int main(int argc, char **argv) {
    if (argc > 1) {
        Processor *processor;
        processor = Processor::getInstance ();
        processor->start(argc, argv);
    }else{
        printf("Usage: Processing ONI_file [display_window]\n");
        printf("\tONI_file: path to the ONI source file\n");
        printf("\tdisplay_window: optional default is 0. Values:\n\t\t\t0 doesn't display any window\n\t\t\t1 displays the video while processing\n");
        printf("\texample: Processing example.oni\n");
        return 0;
    }
}
