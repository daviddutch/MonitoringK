#include "sequence.h"
#include "qdom.h"


Sequence::Sequence(Generators& generators, xn::Player& player) :
    gen(generators),
    g_player(player)
{
    printf("new Sequence\n");
    player.TellFrame(gen.depth.GetName(), this->startFrame);
    update();
}

void Sequence::update() {
    printf("update seq()\n");
    int nbUsers = gen.user.GetNumberOfUsers();

    if (nbUsers==0){
        printf("end update seq with no users\n");
        return;
    }

    for (int user=1; user<=nbUsers;user++){
        //if new user create object
        if(movingObjects.find(user) == movingObjects.end()){
            movingObjects.insert(std::map<int, MovingObject>::value_type(user, MovingObject(user, gen, g_player)));
        }
        movingObjects.find(user)->second.update(); //tells the moving object that there is new data. he can update his self
    }
    printf("end update seq\n");
}

void Sequence::toXML(QDomDocument& doc, QDomElement& movieNode) {
    XnUInt32 endFrameNo;
    g_player.TellFrame(gen.depth.GetName(), endFrameNo);
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

