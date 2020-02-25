#include "mesh.h"

#include "../objloader.h"
#include "../vertex.h"
#include "triangle.h"

#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

//Hit(double time, Vector const &normal)

Hit Mesh::intersect(Ray const &ray) {
    // Replace the return of a NO_HIT by determining the intersection based
    // on the ray and this class's data members.
    int intersect = 0;
    ObjectPtr tri;
    Hit isIntersected(numeric_limits<double>::infinity(), Vector());

    for (size_t i = 0; i < d_tris.size(); i++) {
        tri = d_tris[i];
        Hit hit = tri->intersect(ray);
        if (hit.t < isIntersected.t) {
            isIntersected = hit;
        }
        intersect = 1;
    }
    if (intersect == 1) {
        return isIntersected;
    }
    return Hit::NO_HIT();
}

Mesh::Mesh(string const &filename, Point const &position, Vector const &rotation, Vector const &scale) {
    OBJLoader model(filename);
    d_tris.reserve(model.numTriangles());
    vector<Vertex> vertices = model.vertex_data();
    for (size_t tri = 0; tri != model.numTriangles(); ++tri) {
        Vertex one = vertices[tri * 3];
        Point v0(one.x, one.y, one.z);

        Vertex two = vertices[tri * 3 + 1];
        Point v1(two.x, two.y, two.z);

        Vertex three = vertices[tri * 3 + 2];
        Point v2(three.x, three.y, three.z);

        // Apply non-uniform scaling, rotation and translation to the three points
        // of the triangle (v0, v1, and v2) here.
        v0 = v0 * scale;
        v1 = v1 * scale;
        v2 = v2 * scale;

        // Rotation
        double cos_rotation[3] = {cos(rotation.x), cos(rotation.y), cos(rotation.z)};
        double sin_rotation[3] = {sin(rotation.x), sin(rotation.y), sin(rotation.z)};

        //for vector 0
        Vector tmp0 = v0;
        //first rotation around ox
        tmp0.x = v0.x;
        tmp0.y = v0.y * cos_rotation[0] - v0.z * sin_rotation[0];
        tmp0.z = v0.y * sin_rotation[0] + v0.z * cos_rotation[0];

        v0 = tmp0;

        //second rotation around oy
        tmp0.x = v0.x * cos_rotation[1] + v0.z * sin_rotation[1];
        tmp0.y = v0.y;
        tmp0.z = -v0.x * sin_rotation[1] + v0.z * cos_rotation[1];

        v0 = tmp0;

        //third rotation around oz
        tmp0.x = v0.x * cos_rotation[2] - v0.y * sin_rotation[2];
        tmp0.y = v0.x * sin_rotation[2] + v0.y * cos_rotation[2];
        tmp0.z = v0.z;

        v0 = tmp0;

        //for vector 1
        Vector tmp1 = v1;
        //first rotation around ox
        tmp1.x = v1.x;
        tmp1.y = v1.y * cos_rotation[0] - v1.z * sin_rotation[0];
        tmp1.z = v1.y * sin_rotation[0] + v1.z * cos_rotation[0];

        v1 = tmp1;

        //second rotation around oy
        tmp1.x = v1.x * cos_rotation[1] + v1.z * sin_rotation[1];
        tmp1.y = v1.y;
        tmp1.z = -v1.x * sin_rotation[1] + v1.z * cos_rotation[1];

        v1 = tmp1;

        //third rotation around oz
        tmp1.x = v1.x * cos_rotation[2] - v1.y * sin_rotation[2];
        tmp1.y = v1.x * sin_rotation[2] + v1.y * cos_rotation[2];
        tmp1.z = v1.z;

        v1 = tmp1;

        //for vector 2
        Vector tmp2 = v1;
        //first rotation around ox
        tmp2.x = v2.x;
        tmp2.y = v2.y * cos_rotation[0] - v2.z * sin_rotation[0];
        tmp2.z = v2.y * sin_rotation[0] + v2.z * cos_rotation[0];

        v2 = tmp2;

        //second rotation around oy
        tmp2.x = v2.x * cos_rotation[1] + v2.z * sin_rotation[1];
        tmp2.y = v2.y;
        tmp2.z = -v2.x * sin_rotation[1] + v2.z * cos_rotation[1];

        v2 = tmp2;

        //third rotation around oz
        tmp2.x = v2.x * cos_rotation[2] - v2.y * sin_rotation[2];
        tmp2.y = v2.x * sin_rotation[2] + v2.y * cos_rotation[2];
        tmp2.z = v2.z;

        v2 = tmp2;

        // Translation
        v0 += position;
        v1 += position;
        v2 += position;

        d_tris.push_back(ObjectPtr(new Triangle(v0, v1, v2)));
    }

    cout << "Loaded model: " << filename << " with " <<
         model.numTriangles() << " triangles.\n";
}
