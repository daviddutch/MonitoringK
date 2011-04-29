#include "processor.h"

int main(int argc, char **argv) {
    if (argc > 1) {
        Processor *processor;
        processor = Processor::getInstance ();
        processor->start(argc, argv);
    }else{
        printf("\nIntruder usage : intruder [ONI file]* \n");
        printf("       example : recorder 2011_03_29[15_14_59].oni\n");
        return 0;
    }
}
