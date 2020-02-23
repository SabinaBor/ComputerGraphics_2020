#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray) {
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
     * ray = ray.O + t * ray.D
     * where A = ray.O is the origin of the ray,
     * B = ray.D is the direction of the ray which is a unit vector
     *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (position) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
     * combined intersection of ray-sphere =
     * dot((ray.O + t*ray.D - position),(ray.O + t*ray.D - position)) = r^2
     *
     * expanded: at^2 + bt + c = 0
     * a = dot(ray.D, ray.D)
     * b = 2*dot(ray.D, ray.O - position)
     * c = dot(ray.O - position, ray.O - position) - r^2
     *
     * t = (-b +- sqrt(discriminant))/2a
     * discriminant = b^2 - 4ac
     * discriminant < 0 => no intersection
     * discriminant = 0 => one point intersection
     * discriminant > 0 => 2 points intersection
     *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // Placeholder for actual intersection calculation.
    double t, t1, t2;
    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(ray.O - position);
    double c = (ray.O - position).dot(ray.O - position) - pow(r, 2);
    double discriminant = pow(b, 2) - 4 * a * c;
    if (discriminant < 0) {
        return Hit::NO_HIT();
    } else if (discriminant == 0) {
        t = -(b / 2 * a);
    } else {
        t1 = (-b + sqrt(discriminant)) / 2 * a;
        t2 = (-b - sqrt(discriminant)) / 2 * a;
        (t1 < 0) ? (t = t2) : ((t2 < 0) ? (t = t1) : (t = min(t1, t2)));
    }
    Point ray1 = ray.O + t * ray.D;
    Vector N = (ray1 - position).normalized();
    return Hit(t, N);
}


Sphere::Sphere(Point const &pos, double radius)
        :
        position(pos),
        r(radius) {}
