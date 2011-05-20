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
    std::string destination = "";   //destination to save ONI
    long duration;                  //recording duration in second
    int hour = 0, minute = 0;       //time set by user to start recording
    bool isTimeStart = false;       //is user set a time to start recording
    time_t rawtime;                 //current time
    struct tm *timeinfo;            //current time
    QStringList list;               //use to split string

    //Check args
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
        printf("Usage: recorder duration_sec [destination_path] [start_time]\n");
        printf("\tduration_sec: duration time of the recording in seconds\n");
        printf("\tdestination_path: Optional path in which the oni file will be recorded\n");
        printf("\tstart_time: Optional time at which the recording must start\n");
        printf("\texample: recorder 120 /home/user/ 12:30 \n");
        printf("\texample: recorder 60 \n");
        return 0;
    }

    //wait until time start is now or not specify
    while(isTimeStart){
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        if((timeinfo->tm_hour == hour && timeinfo->tm_min == minute) )
            break;
        printf("current time: %02d:%02d\n", timeinfo->tm_hour, timeinfo->tm_min);
        sleep(30);
    }

    //Create the recorder and start it
    OniRecorder recorder(duration, destination);
    recorder.init();
    recorder.start();

    return 1;
}
