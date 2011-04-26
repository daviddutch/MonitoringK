/****************************************************************************
*                                                                           *
*   Nite 1.3 - Players Sample                                               *
*                                                                           *
*   Author:     Oz Magal                                                    *
*                                                                           *
****************************************************************************/

/****************************************************************************
*                                                                           *
*   Nite 1.3	                                                            *
*   Copyright (C) 2006 PrimeSense Ltd. All Rights Reserved.                 *
*                                                                           *
*   This file has been provided pursuant to a License Agreement containing  *
*   restrictions on its use. This data contains valuable trade secrets      *
*   and proprietary information of PrimeSense Ltd. and is protected by law. *
*                                                                           *
****************************************************************************/

#include <QtCore/QCoreApplication>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <GL/glut.h>
#include "SceneDrawer.h"
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include <QStringList>
#include <qfile.h>
#include <QTextStream>
#include <qdom.h>
#include <string>
#include "processor.h"
//#include "xmlwriter.h"

int main(int argc, char **argv)
{
    if (argc > 1) {
        Processor *processor;
        processor = Processor::getInstance ();
        processor->start(argc, argv);
    }else{
        printf("\nIntruder usage : intruder [ONI file]* \n");
        printf("       example : recorder 2011_03_29[15_14_59].oni\n");
        return 0;
    }
}
