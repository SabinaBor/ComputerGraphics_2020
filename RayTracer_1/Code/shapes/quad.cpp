#include "quad.h"

#include <limits>

using namespace std;

Hit Quad::intersect(Ray const &ray)
{
    // placeholder
    return Hit::NO_HIT();
}

Quad::Quad(Point const &v0,
           Point const &v1,
           Point const &v2,
           Point const &v3)
{
    // Store and/or process the points defining the quad here.
}
