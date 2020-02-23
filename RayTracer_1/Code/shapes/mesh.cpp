#include "mesh.h"

#include "../objloader.h"
#include "../vertex.h"
#include "triangle.h"

#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

Hit Mesh::intersect(Ray const &ray)
{
    // Replace the return of a NO_HIT by determining the intersection based
    // on the ray and this class's data members.
    Point ray1 = ray.O;
    return Hit::NO_HIT();
}

Mesh::Mesh(string const &filename, Point const &position, Vector const &rotation, Vector const &scale)
{
    OBJLoader model(filename);
    d_tris.reserve(model.numTriangles());
    vector<Vertex> vertices = model.vertex_data();
    for (size_t tri = 0; tri != model.numTriangles(); ++tri)
    {
        Vertex one = vertices[tri * 3];
        Point v0(scale.x * one.x, scale.y * one.y, scale.z * one.z);

        Vertex two = vertices[tri * 3 + 1];
        Point v1(scale.x * two.x, scale.y * two.y, scale.z * two.z);

        Vertex three = vertices[tri * 3 + 2];
        Point v2(scale.x * three.x, scale.y * three.y, scale.z * three.z);

        // Apply non-uniform scaling, rotation and translation to the three points
        // of the triangle (v0, v1, and v2) here.



        // Rotation
        // ...

        // Translation
        // ...

        d_tris.push_back(ObjectPtr(new Triangle(v0, v1, v2)));
    }

    cout << "Loaded model: " << filename << " with " <<
        model.numTriangles() << " triangles.\n";
}
