#include "sequence.h"
#include "qdom.h"


Sequence::Sequence(xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator, xn::Player& player)
{
    printf("new Sequence\n");
    userGenerator  = uGenerator;
    depthGenerator = dGenerator;
    imageGenerator = iGenerator;
    g_player = player;
    nObjects = 0;
    player.TellFrame(depthGenerator.GetName(), this->startFrame);
    update();
}

void Sequence::update() {
    //printf("update seq()\n");
    int nbUsers = userGenerator.GetNumberOfUsers();

    nObjects = (nbUsers>nObjects) ? nbUsers : nObjects;
    if (nbUsers==0){
        return;
    }

    for (int user=1; user<=nbUsers;user++){
        //if new user create object
        if(movingObjects.find(user) == movingObjects.end()){
            movingObjects[user] = MovingObject(user, userGenerator, depthGenerator, imageGenerator, g_player);
        }
        movingObjects[user].update(); //tells the moving object that there is new data. he can update his self
    }
    //printf("end update seq\n");
}

void Sequence::toXML(QDomDocument& doc, QDomElement& movieNode) {
    XnUInt32 endFrameNo;
    g_player.TellFrame(depthGenerator.GetName(), endFrameNo);
    printf("**** sequence XML writing *****\n");
    QDomElement sequenceNode = doc.createElement("sequence");
    sequenceNode.setAttribute("startFrameNo",startFrame);
    sequenceNode.setAttribute("endFrameNo",endFrameNo);
    movieNode.appendChild(sequenceNode);
   for( std::map<int, MovingObject>::iterator ii=movingObjects.begin(); ii!=movingObjects.end(); ++ii) {
       //cout << (*ii).first << ": " << (*ii).second << endl;
       (*ii).second.toXML(doc, sequenceNode);
   }

}

