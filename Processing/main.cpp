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
        printf("\nProcessor usage : Processing [ONI file]* [display result]\n");
        printf("       example : recorder 2011_03_29[15_14_59].oni 1\n");
        printf("       example : recorder 2011_03_29[15_14_59].oni\n");
        return 0;
    }
}
