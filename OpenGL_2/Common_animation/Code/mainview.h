#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "math.h"

#include <QImage>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector>
#include <QVector3D>

#include <memory>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger debugLogger;
    QTimer timer; // Timer used for animation.

    QOpenGLShaderProgram phongShaderProgram;

    // Uniforms for the Phong shader program.
    GLint uniformModelViewTransformPhong;
    GLint uniformProjectionTransformPhong;
    GLint uniformNormalTransformPhong;

    GLint uniformMaterialPhong;
    GLint uniformLightPositionPhong;
    GLint uniformLightColorPhong;

    GLint uniformTextureSamplerPhong;

    // Buffers
    GLuint meshVAO[4];
    GLuint meshVBO[4];
    GLuint meshSize[4];

    // Texture
    GLuint textureName[4];

    // Transforms
    float scale = 1.0F;
    QVector3D rotation;
    QMatrix4x4 projectionTransform;
    QMatrix3x3 meshNormalTransform[4];
    QMatrix4x4 meshTransform[4];

    float xTranslate[4];
    float yTranslate[4];
    float zTranslate[4];
    float xRotation[4];
    float yRotation[4];
    float zRotation[4];
    float angle[3];
    float time = 0;
    float k = (M_PI / 180.0);
    // Phong model constants.
    QVector4D material = {0.5F, 0.5F, 0.5F, 5.0F};
    QVector3D lightPosition = {0.0F, 100.0F, 0.0F};
    QVector3D lightColor = {1.0F, 1.0F, 1.0F};

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    // Functions for widget input events.
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(int scale);
    void setShadingMode(ShadingMode shading);

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

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();
    void loadMesh(QString name, int i);

    // Loads texture data into the buffer with the name textureName.
    void loadTextures();
    void loadTexture(QString file, GLuint texturePtr);

    void destroyModelBuffers();

    void updateProjectionTransform();
    void updateModelTransforms();
    void rotateModel(int index, QVector3D coord);

    void updatePhongUniforms(int i);

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);

    // The current shader to use.
    ShadingMode currentShader = PHONG;
};

#endif // MAINVIEW_H
