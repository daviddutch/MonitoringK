#include "generators.h"


Generators::Generators(xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator) :
    user(uGenerator),
    depth(dGenerator),
    image(iGenerator)
{

}
