#include "triangle.h"

/*
 * Write the function using Möller-Trumbore algorithm
 * ray = ray.O + t*ray.D
 * We have a triangle with sides a, b, c
 * ray = A + u(B-A) + v(C-A) where B - A = v1 - v0
 * and C - A = v2 - v0
 * A + u(B-A) + v(C-A) = ray.O + t*ray.D
 * ray.O - A = -t*ray.D + u(B-A) + v(C-A)
 * */

Hit Triangle::intersect(Ray const &ray) {
    /* create two sides of the triangle AB and AC */
    Vector v0v1 = v1 - v0;
    Vector v0v2 = v2 - v0;
    Vector pvec, tvec, qvec;
    double t, u, v; // unknown variables
    double determinant, indeterminant;
    pvec = ray.D.cross(v0v2);

    determinant = v0v1.dot(pvec);
    if (determinant < EPSILON && determinant > -EPSILON) // triangle and ray are parallel
        return Hit::NO_HIT();

    indeterminant = 1 / determinant;
    tvec = ray.O - v0;
    u = tvec.dot(pvec) * indeterminant;
    if (u < 0 || u > 1) return Hit::NO_HIT();

    qvec = tvec.cross(v0v1);
    v = ray.D.dot(qvec) * indeterminant;
    if (v < 0 || u + v > 1) return Hit::NO_HIT();

    t = v0v2.dot(qvec) * indeterminant;

    Point ray1 = ray.O + t * ray.D;
    Vector N = ray1.normalized();

    return Hit(t, N);
}

Triangle::Triangle(Point const &v0,
                   Point const &v1,
                   Point const &v2)
        :
        v0(v0),
        v1(v1),
        v2(v2) {
    // Calculate the surface normal here and store it in the N,
    // which is declared in the header. It can then be used in the intersect function.
}
