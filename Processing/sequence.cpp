#include "sequence.h"
#include "qdom.h"
#include <math.h>

Sequence::Sequence(Generators& generators, std::string d) :
    gen(generators),
    dir(d)
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
        int indexUser=-1;
        XnPoint3D com;
        Metric newMetric;
        gen.user.GetCoM(user, com);
        if (com.Z!=0)
            newMetric = computeMetrics(user, com);
        //check if user exist
        for(int i=0; i < movingObjects.size(); i++) {
            if (movingObjects[i].getXnId()==user){
                indexUser = i;
                if (com.Z!=0) {
                    Metric oldMetric = movingObjects[indexUser].getMetric();
                    float evolvHeight = abs(newMetric.height-oldMetric.height)/newMetric.height;
                    printf("Pourcentage evolv height %f\n", evolvHeight);

                    float evolvWidth = abs(newMetric.width-oldMetric.width)/newMetric.width;
                    printf("Pourcentage evolv width %f\n", evolvWidth);

                    //if (evolvHeight>0.6 || evolvWidth>0.6){
                    if (evolvHeight>0.6){
                        movingObjects[indexUser].setXnId(0);
                        movingObjects.push_back(MovingObject(user, gen, dir));
                        indexUser = movingObjects.size() - 1;
                    }

                    /*
                    XnPoint3D comUser, comNew;
                    comUser = movingObjects.at(user).getCom();
                    comNew = getComByUser(movingObjects.at(user).getXnId());
                    if(comNew.X<-0.1 || comNew.X >0.1){
                        //printf("user COM : (%f;%f;%f)\n", comUser.X, comUser.Y, comUser.Z);
                        //printf("new  COM : (%f;%f;%f)\n", comNew.X, comNew.Y, comNew.Z);
                        //if(isTwoPointClose(comUser, comNew))
                        //    printf("the two points are close !\n");
                    }
                    */
                }
                break;
            }
        }

        //if new user create object
        if(indexUser<0 && com.Z!=0){
            movingObjects.push_back(MovingObject(user, gen, dir));
            indexUser = movingObjects.size() - 1;
        }
        if (com.Z!=0){
            movingObjects[indexUser].setMetric(newMetric);
            movingObjects[indexUser].update(); //tells the moving object that there is new data. he can update his self
        }
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
    for(int i=0; i < movingObjects.size(); i++) {
        movingObjects[i].toXML(doc, sequenceNode);
    }

}

XnPoint3D Sequence::getComByUser(int id)
{
    XnPoint3D com, com2;
    gen.user.GetCoM(id, com);
    //XnPoint3D com2;
    //gen.depth.ConvertProjectiveToRealWorld(1, &com, &com2);

    return com;
}

bool Sequence::isTwoPointClose(XnPoint3D p1, XnPoint3D p2)
{
    //sqrt[(Xa-Xb)²+(Ya-Yb)²+(Za-Zb)²]
    float dist = sqrt(pow(p1.X-p2.X,2) + pow(p1.Y-p2.Y,2) + pow(p1.Z-p2.Z,2));
    return (dist < 500);
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

    for(int x=i;x<XN_VGA_X_RES;x--){
        if (nbOther==0){
            lcom.X = x;
            lcom.Y = j;
            lcom.Z = pDepthMap[j * XN_VGA_X_RES + x];
            break;
         }

        if (userPix[j * XN_VGA_X_RES + x ] != userId) {
            nbOther--;
         }
    }
    nbOther = 1;
    for(int x=i;x<XN_VGA_X_RES;x++){
        if (nbOther==0){
            rcom.X = x;
            rcom.Y = j;
            rcom.Z = pDepthMap[j * XN_VGA_X_RES + x];
            break;
        }
        if (userPix[j * XN_VGA_X_RES + x ] != userId) {
            nbOther--;
        }
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

    gen.depth.ConvertProjectiveToRealWorld(1, &lcom, &lcom);
    gen.depth.ConvertProjectiveToRealWorld(1, &rcom, &rcom);

    metric.width = rcom.X - lcom.X;

    if(lcom.X > 0.1 && rcom.X > 0.1){
        //printf("real world %d left : (%f, %f, %f)\n", id, lcom.X, lcom.Y, lcom.Z);
        //printf("real world %d right : (%f, %f, %f)\n", id, rcom.X, rcom.Y, rcom.Z);
        //float dist = getDistance(lcom, rcom);
        metric.width = rcom.X - lcom.X;
        //printf("real world %d distance : %f\n", id, dist);
    }


    //----computes the height of the object
    XnPoint3D top;
    XnPoint3D bottom;
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

    gen.depth.ConvertProjectiveToRealWorld(1, &top, &top);
    gen.depth.ConvertProjectiveToRealWorld(1, &bottom, &bottom);

    //float dist = getDistance(top, bottom);
    //printf("real world %d height with dist : %f height with diff : %f\n", id, dist, top.Y-bottom.Y);

    metric.height = top.Y-bottom.Y;

    return metric;
}
