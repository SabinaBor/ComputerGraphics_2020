#include "triangle.h"

Hit Triangle::intersect(Ray const &ray)
{
    return Hit::NO_HIT(); // placeholder
}

Triangle::Triangle(Point const &v0,
                   Point const &v1,
                   Point const &v2)
:
    v0(v0),
    v1(v1),
    v2(v2),
    N()
{
    // Calculate the surface normal here and store it in the N,
    // which is declared in the header. It can then be used in the intersect function.
}
