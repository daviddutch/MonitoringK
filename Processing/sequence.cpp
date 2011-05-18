#include "sequence.h"
#include "qdom.h"
#include <math.h>

Sequence::Sequence(Generators& generators, std::string d) :
    gen(generators),
    dir(d)
{
    gen.player.TellFrame(gen.image.GetName(), this->startFrame);
    update();
}

void Sequence::update() {
    int nbUsers = gen.user.GetNumberOfUsers();

    if (nbUsers==0){
        printf("end update seq with no users\n");
        return;
    }

    Metric newMetric;
    newMetric.height = 0;
    newMetric.width = 0;

    for (int user=1; user<=nbUsers;user++){
        int indexUser=-1;
        XnPoint3D com;
        gen.user.GetCoM(user, com);
        for (int i=0; i < movingObjects.size(); i++) {
            if (movingObjects[i].getXnId()==user) {
                indexUser = i;
                movingObjects[i].update(newMetric); //tells the moving object that there is new data. he can update his self
            }
        }
        if (indexUser<0 && (com.Z > 0.001)) {
            for (int j=0; j < movingObjects.size(); j++) {
                if(movingObjects[j].getState() == SEPERATED){
                    movingObjects[j].setState(OUT_OF_SIGHT);
                    movingObjects[j].setXnId(0);
                }
            }
            movingObjects.push_back(MovingObject(user, gen, dir));
            indexUser = movingObjects.size() - 1;
            movingObjects[indexUser].update(newMetric); //tells the moving object that there is new data. he can update his self

        }
    }
}

void Sequence::toXML(TiXmlElement* movieNode) {
    XnUInt32 endFrameNo;
    gen.player.TellFrame(gen.image.GetName(), endFrameNo);
    TiXmlElement * sequenceNode = new TiXmlElement("sequence");
    sequenceNode->SetAttribute("startFrameNo",startFrame);
    sequenceNode->SetAttribute("endFrameNo",endFrameNo);
    movieNode->LinkEndChild(sequenceNode);
    for(int i=0; i < movingObjects.size(); i++) {
        movingObjects[i].toXML(sequenceNode);
    }
}
