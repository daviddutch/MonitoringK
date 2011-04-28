#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdio.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <vector>
#include "defs.h"
#include "generators.h"
#include "sequence.h"
#include "qdom.h"
#include <iostream>
#include <string>

class Processor
{
public:
    int start(int argc, char **argv);
    static Processor *getInstance();
    static void XN_CALLBACK_TYPE NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie);
    static void XN_CALLBACK_TYPE LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie);
private:
    Processor();
    static Processor *instance;

    Generators* gen;
    xn::Context context;
    int nUser;
    bool hasUserInSight;
    Sequence* sequence;
    QDomDocument doc;
    QDomElement movieNode;
    std::string dateStart;
    std::string fileName;
    const XnChar* strNodeName;

    void createXML();
    void writeXML();

    static void CleanupExit();
    void DrawProjectivePoints(XnPoint3D& ptIn, int width, double r, double g, double b);
    static void glutDisplay (void);
    static void glutIdle (void);
    static void glutKeyboard (unsigned char key, int x, int y);
    void glInit (int * pargc, char ** argv);
};

#endif // PROCESSOR_H
