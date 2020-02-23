#include "cylinder.h"

#include <cmath>

using namespace std;

Hit Cylinder::intersect(Ray const &ray)
{
    return Hit::NO_HIT(); // placeholder
}

Cylinder::Cylinder(Point const &pos, Vector const &direction, double radius)
:
    position(pos),
    direction(direction),
    radius(radius)
{}
