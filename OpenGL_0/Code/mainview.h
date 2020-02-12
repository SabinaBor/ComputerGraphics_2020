#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "vertex.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QVector3D>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLShaderProgram shaderProgram;

public:
    enum ShadingMode: GLuint{
        PROGRAM
    };

    MainView(QWidget *parent = 0);
    ~MainView();

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

    vertex createVertex(float x, float y, float red, float green, float blue);

private:
    QOpenGLDebugLogger debugLogger;
    GLuint meshVBO;
    GLuint meshVAO;

    vertex vert[3];

private slots:
    void onMessageLogged(QOpenGLDebugMessage Message);
};

#endif // MAINVIEW_H
