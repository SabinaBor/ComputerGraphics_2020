#include "quad.h"


#include <limits>

using namespace std;

Hit Quad::intersect(Ray const &ray) {
    // placeholder
    Hit tmp(numeric_limits<double>::infinity(), Vector());
    Hit hit[2] = {tri1->intersect(ray), tri2->intersect(ray)};
    if (hit[0].t < tmp.t) {
        return hit[0];
    }
    if (hit[1].t < tmp.t) {
        return hit[1];
    }
    return Hit::NO_HIT();
}

Quad::Quad(Point const &v0,
           Point const &v1,
           Point const &v2,
           Point const &v3) {
    // Store and/or process the points defining the quad here.
    tri1 = new Triangle(v0, v1, v2);
    tri2 = new Triangle(v0, v2, v3);
}
