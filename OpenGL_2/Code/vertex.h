#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

struct Vertex
{
    QVector3D coords;
    QVector3D normal;
    QVector2D texCoords;

    Vertex(QVector3D coords, QVector3D normal)
        :
          coords(coords),
          normal(normal)
    {}
};

#endif // VERTEX_H

