#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <string>
#include <QStringList>

#include "onirecorder.h"



int main(int argc, char *argv[]) {
    std::string destination;
    long duration;              //recording duration
    int hour = 0, minute = 0;   //use to start time
    bool isTimeStart = false;
    time_t rawtime;
    struct tm *timeinfo;
    QStringList list;

    if (argc > 1) {
        switch (argc) {
            case 4:
                isTimeStart = true;
                //Split the string to hour and minute var
                list = QString(argv[3]).split(":");
                if (list.count()>2){
                    printf("error while parsing time \n");
                    return 0;
                }
                hour =  atoi(list.takeFirst().toStdString().c_str());
                minute =  atoi(list.takeFirst().toStdString().c_str());
            case 3:
                destination = argv[2];
            case 2:
                duration = atoi(argv[1]);
                break;
        }
    } else {
        printf("\nRecorder usage : recorder [duration sec]* [destination ONI] [Time start] \n");
        printf("       example : recorder 120 /home/user/ 12:30 \n");
        printf("       example : recorder 60 \n");
        return 0;
    }

    printf("the recording will start ");
    if(isTimeStart) {
        printf("at %02d:%02d", hour, minute);
    } else {
        printf("now");
    }
    printf(" during %d sec and will be saved in ", duration);

    if(destination != "") {
        printf("%s", destination.c_str());
    } else {
        printf("the same directory");
    }
    printf("\n");

    //wait until time start is now or not specify
    while(isTimeStart){
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        if((timeinfo->tm_hour == hour && timeinfo->tm_min == minute) )
            break;
        printf("current time: %02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min);
        sleep(30);
    }


    OniRecorder recorder(duration, destination);

    recorder.init();

    recorder.start();


    return 1;
}
