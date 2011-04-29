#include "sequence.h"
#include "qdom.h"
#include <math.h>

Sequence::Sequence(Generators& generators) :
    gen(generators)
{
    printf("new Sequence\n");
    gen.player.TellFrame(gen.depth.GetName(), this->startFrame);
    update();
}

void Sequence::update() {
    //printf("update seq()\n");
    int nbUsers = gen.user.GetNumberOfUsers();

    if (nbUsers==0){
        printf("end update seq with no users\n");
        return;
    }

    for (int user=1; user<=nbUsers;user++){
        //Check user's COM
        if(movingObjects.find(user) != movingObjects.end()){
            XnPoint3D comUser, comNew;
            comUser = movingObjects.at(user).getCom();
            comNew = getComByUser(movingObjects.at(user).getId());
            if(comNew.X<-0.1 || comNew.X >0.1){
                //printf("user COM : (%f;%f;%f)\n", comUser.X, comUser.Y, comUser.Z);
                //printf("new  COM : (%f;%f;%f)\n", comNew.X, comNew.Y, comNew.Z);
                //if(isTwoPointClose(comUser, comNew))
                //    printf("the two points are close !\n");
            }
        }
        //if new user create object
        if(movingObjects.find(user) == movingObjects.end()){
            movingObjects.insert(std::map<int, MovingObject>::value_type(user, MovingObject(user, gen)));

            //movingObjects[user] = MovingObject(user, userGenerator, depthGenerator, imageGenerator, g_player);
        }
        movingObjects.find(user)->second.update(); //tells the moving object that there is new data. he can update his self
    }
    //printf("end update seq\n");
}

void Sequence::toXML(QDomDocument& doc, QDomElement& movieNode) {
    XnUInt32 endFrameNo;
    gen.player.TellFrame(gen.depth.GetName(), endFrameNo);
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

XnPoint3D Sequence::getComByUser(int id)
{
    XnPoint3D com, com2;
    gen.user.GetCoM(id, com);
    //XnPoint3D com2;
    gen.depth.ConvertProjectiveToRealWorld(1, &com, &com2);

    return com2;
}

bool Sequence::isTwoPointClose(XnPoint3D p1, XnPoint3D p2)
{
    //sqrt[(Xa-Xb)²+(Ya-Yb)²+(Za-Zb)²]
    float dist = sqrt(pow(p1.X-p2.X,2) + pow(p1.Y-p2.Y,2) + pow(p1.Z-p2.Z,2));
    return (dist < 500);
}

