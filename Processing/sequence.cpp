#include "sequence.h"
#include "qdom.h"
#include <math.h>

Sequence::Sequence(Generators& generators, std::string d) :
    gen(generators),
    dir(d)
{
    //printf("new Sequence\n");
    gen.player.TellFrame(gen.image.GetName(), this->startFrame);
    update();
}

void Sequence::update() {
    //printf("update seq()\n");
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
        if (indexUser<0 && com.Z!=0.0) {
            printf("CREATE A NEW USER \n");
            for (int j=0; j < movingObjects.size(); j++) {
                if(movingObjects[j].getState() == SEPERATED){
                    printf("%d object is separated -> OUT_OF_SIGHT \n", movingObjects[j].getXnId());
                    movingObjects[j].setState(OUT_OF_SIGHT);
                    movingObjects[j].setXnId(0);
                }
            }
            movingObjects.push_back(MovingObject(user, gen, dir));
            indexUser = movingObjects.size() - 1;
            movingObjects[indexUser].update(newMetric); //tells the moving object that there is new data. he can update his self

        }
    }











    return;

    for (int user=1; user<=nbUsers;user++){
        int indexUser=-1;
        XnPoint3D com;
        Metric newMetric;
        gen.user.GetCoM(user, com);
        //printf("USER %d\n", user);
        if (com.Z>0.1){
            newMetric = computeMetrics(user, com);
            //printf("new Metric width: %f\n",newMetric.width);
            if (newMetric.width > 0.0){
                //check if user exist
                for(int i=0; i < movingObjects.size(); i++) {
                    if (movingObjects[i].getXnId()==user){
                        indexUser = i;

                        if (!isSameObject(indexUser, com, newMetric)){

                            if(movingObjects[indexUser].getFrames().size()>1){
                                newMetric.validWidth = (movingObjects[indexUser].getMetric().validWidth < 0.1) ? movingObjects[indexUser].getMetric().width : movingObjects[indexUser].getMetric().validWidth;
                            }
                            //printf("\t USER %d countdown.. %f (%f)\n", indexUser, newMetric.validWidth, movingObjects[indexUser].getMetric().width);
                            if(movingObjects[indexUser].isValidWidthCountDown()){
                                //printf("THERE IS PROBABLY A NEW USER \n");
                                movingObjects[indexUser].setXnId(0);
                                movingObjects[indexUser].resetValidWidth();
                                movingObjects.push_back(MovingObject(user, gen, dir));
                                indexUser = movingObjects.size() - 1;
                                newMetric.validWidth = 0;
                            }
                        }else{
                            //printf("\t valid width:%f\n", newMetric.width);
                            movingObjects[indexUser].resetValidWidthCountDown();
                            newMetric.validWidth = newMetric.width;
                        }
                        break;
                    }
                }
                //if new user create object
                if(indexUser<0){
                    //printf("CREATE A NEW USER \n");
                    movingObjects.push_back(MovingObject(user, gen, dir));
                    indexUser = movingObjects.size() - 1;
                }

                movingObjects[indexUser].update(newMetric); //tells the moving object that there is new data. he can update his self
            }
        }

    }
    //printf("end update seq\n");
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
bool Sequence::isSameObject(int indexUser, XnPoint3D com, Metric metric){
    Metric oldMetric = movingObjects[indexUser].getMetric();
    if(metric.height==0.0 || metric.width==0.0)
        return true;
    float evolvHeight = abs(metric.height-oldMetric.height)/metric.height;
    //printf("Object %d:\n\tPourcentage evolv height %f\n", indexUser, evolvHeight);

    float evolvWidth = abs(metric.width-oldMetric.validWidth)/metric.width;
    //printf("\tPourcentage evolv width %f / (old:%f, new:%f)\n", evolvWidth , oldMetric.width, metric.width);

    float dist = getDistance(com, movingObjects[indexUser].getCom());
    //printf("\tDist between com %f\n", dist);

    //if (evolvHeight>0.6 || evolvWidth>0.6){
    if (evolvWidth>0.6){
        return false;
    }
    return true;
}
float Sequence::getDistance(XnPoint3D p1, XnPoint3D p2){
    return sqrt(pow(p1.X-p2.X,2) + pow(p1.Y-p2.Y,2) + pow(p1.Z-p2.Z,2));
}
Metric Sequence::computeMetrics(XnUserID userId, XnPoint3D com) {
    Metric metric;

    xn::SceneMetaData sceneMetaData;
    gen.user.GetUserPixels(userId, sceneMetaData);
    unsigned short *userPix = (unsigned short*)sceneMetaData.Data();

    XnPoint3D com2;
    gen.depth.ConvertRealWorldToProjective(1, &com, &com2);
    const XnDepthPixel* pDepthMap = gen.depth.GetDepthMap();

    int i = (int)com2.X;
    int j = (int)com2.Y;

    //----computes the with of the object on the center of gravity
    XnPoint3D lcom;
    XnPoint3D rcom;
    int nbOther = 1;

    float f = pDepthMap[j * XN_VGA_X_RES + i];
    //printf("com.Z : %f pdepthZ : %f\n", com.Z, f);
    if(f<0.1){
        metric.width = -1;
        return metric;
    }
    lcom.X = 0;
    lcom.Y = j;
    lcom.Z = com.Z;
    for(int x=i;x<XN_VGA_X_RES;x--){
        if (nbOther==0){
            lcom.X = x+2;
            float f = pDepthMap[j * XN_VGA_X_RES + x+2];
            lcom.Z = f;
            break;
         }
        if (userPix[j * XN_VGA_X_RES + x ] != userId) {
            nbOther--;
         }
    }
    nbOther = 1;
    rcom.X = XN_VGA_X_RES-1;
    rcom.Y = j;
    rcom.Z = com.Z;
    for(int x=i;x<XN_VGA_X_RES;x++){
        if (nbOther==0){
            rcom.X = x-2;
            float f = pDepthMap[j * XN_VGA_X_RES + x-2];
            rcom.Z = f;
            break;
        }
        if (userPix[j * XN_VGA_X_RES + x ] != userId) {
            nbOther--;
        }
    }

    if(lcom.X<=20 || rcom.X>=(XN_VGA_X_RES-20) ){
        //printf("METRIC LIMIT WIDTH %d\n", userId);
        metric.width = -1.0;
        return metric;
    }


    /*
    Rect rect;
    rect.top       = rcom.Y-5;
    rect.right     = rcom.X;
    rect.bottom    = rcom.Y+5;
    rect.left      = lcom.X;

    XnUInt32 nFrame;
    gen.player.TellFrame(gen.depth.GetName(), nFrame);
    std::ostringstream file2d;
    file2d << dir << "/2D/metric-" << nFrame << ".png";
    outputImage(rect, file2d);
    std::ostringstream file3d;
    file3d << dir << "/3D/metric-" << nFrame << ".png";
    outputDepth(rect, file3d);
    */

    XnPoint3D lcom2;
    XnPoint3D rcom2;

    gen.depth.ConvertProjectiveToRealWorld(1, &lcom, &lcom2);
    gen.depth.ConvertProjectiveToRealWorld(1, &rcom, &rcom2);

    if(lcom2.Z > 0.1 && rcom2.Z > 0.1){
        //printf("real world %d left : (%f, %f, %f)\n", id, lcom.X, lcom.Y, lcom.Z);
        //printf("real world %d right : (%f, %f, %f)\n", id, rcom.X, rcom.Y, rcom.Z);
        //float dist = getDistance(lcom, rcom);
        metric.width = rcom2.X - lcom2.X;
        //printf("real world %d distance : %f\n", id, dist);
    }else{
        printf("ERROR METRIC\n");
        printf("%d real world right : (%f, %f, %f)\n", userId, rcom.X, rcom.Y, rcom.Z);
        printf("%d real world 2 right : (%f, %f, %f)\n", userId, rcom2.X, rcom2.Y, rcom2.Z);
        printf("%d real world left : (%f, %f, %f)\n", userId, lcom.X, lcom.Y, lcom.Z);
        printf("%d real world 2 left : (%f, %f, %f)\n", userId, lcom2.X, lcom2.Y, lcom2.Z);
        printf("%d com  world : (%f, %f, %f)\n", userId, com.X, com.Y, com.Z);
    }


    //----computes the height of the object
    XnPoint3D top;
    top.X = i;
    top.Y = 0;
    top.Z = com.Z;
    XnPoint3D bottom;
    bottom.X = i;
    bottom.Y = XN_VGA_Y_RES-1;
    bottom.Z = com.Z;
    //top
    for (int y=0; y<XN_VGA_Y_RES; y++){
        for(int x=0;x<XN_VGA_X_RES;x++){
            if (userPix[y * XN_VGA_X_RES + x ] == userId) {
                top.X = x;
                top.Y = y;
                top.Z = pDepthMap[y * XN_VGA_X_RES + x];
                y = XN_VGA_Y_RES; //make it exit the outer loop
                break;
            }
        }
    }
    //bottom
    for (int y=XN_VGA_Y_RES-1; y>=0; y--){
        for(int x=XN_VGA_X_RES-1;x>=0;x--){
            if (userPix[y * XN_VGA_X_RES + x ] == userId) {
                bottom.X = x;
                bottom.Y = y;
                bottom.Z = pDepthMap[y * XN_VGA_X_RES + x];
                y = -1; //make it exit the outer loop
                break;
            }
         }
     }
    //printf("Top %d : (%f, %f, %f)\n", id, top.X, top.Y, top.Z);
    //printf("Bottom %d : (%f, %f, %f)\n", id, bottom.X, bottom.Y, bottom.Z);


    if(top.X<=10 || bottom.X>=(XN_VGA_Y_RES-10) ){
        //printf("METRIC LIMIT HEIGHT %d\n", userId);
        metric.width = -1.0;
        return metric;
    }

    gen.depth.ConvertProjectiveToRealWorld(1, &top, &top);
    gen.depth.ConvertProjectiveToRealWorld(1, &bottom, &bottom);

    //float dist = getDistance(top, bottom);
    //printf("real world %d height with dist : %f height with diff : %f\n", id, dist, top.Y-bottom.Y);

    metric.height = top.Y-bottom.Y;

    return metric;
}
