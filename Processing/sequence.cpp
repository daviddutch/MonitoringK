#include "sequence.h"


Sequence::Sequence(int startFrame, xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator)
{
    printf("Sequence\n");
    this->startFrame = startFrame;
    userGenerator  = uGenerator;
    depthGenerator = dGenerator;
    imageGenerator = iGenerator;
    nObjects = 0;
    update();
}

void Sequence::update() {
    printf("update seq()\n");
    int nbUsers = userGenerator.GetNumberOfUsers();

    nObjects = (nbUsers>nObjects) ? nbUsers : nObjects;
    if (nbUsers==0){
        return;
    }

    for (int user=1; user<=nbUsers;user++){
        //if new user create object
        if(movingObjects.find(user) == movingObjects.end()){
            movingObjects[user] = MovingObject(user, userGenerator, depthGenerator, imageGenerator);
        }
        movingObjects[user].update(); //tells the moving object that there is new data. he can update his self
    }
    printf("end update seq\n");
}

void Sequence::toXML() {
   printf("\n\n<sequence startFrame=\"%d\">\n", startFrame);
   for( std::map<int, MovingObject>::iterator ii=movingObjects.begin(); ii!=movingObjects.end(); ++ii) {
       //cout << (*ii).first << ": " << (*ii).second << endl;
       (*ii).second.toXML();
   }
    printf("</sequence>\n");
}

