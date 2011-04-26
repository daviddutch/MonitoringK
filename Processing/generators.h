#ifndef GENERATORS_H
#define GENERATORS_H

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

class Generators
{
public:
    Generators(xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator);
    xn::UserGenerator& user;
    xn::DepthGenerator& depth;
    xn::ImageGenerator& image;
};

#endif // GENERATORS_H
