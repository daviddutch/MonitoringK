#include "generators.h"


Generators::Generators(xn::UserGenerator& uGenerator, xn::DepthGenerator& dGenerator, xn::ImageGenerator& iGenerator, xn::Player& pGenerator) :
    user(uGenerator),
    depth(dGenerator),
    image(iGenerator),
    player(pGenerator)
{

}
