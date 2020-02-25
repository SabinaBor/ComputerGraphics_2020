#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere : public Object {
public:
    Sphere(Point const &pos, double radius);

    virtual Hit intersect(Ray const &ray);

    Point const position;
    double const r;
};

int solveQuadratic(const double &a, const double &b, const double &c, double &t1, double &t2);

#endif
