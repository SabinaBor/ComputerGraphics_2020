#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"
#include "triangle.h"

class Quad : public Object {
public:
    Quad(Point const &v0,
         Point const &v1,
         Point const &v2,
         Point const &v3);

    Triangle *tri1, *tri2;

    virtual Hit intersect(Ray const &ray);
};

#endif
