#include "mainview.h"
#include "model.h"

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    qDebug() << "MainView destructor";

    makeCurrent();

    for(int i = 0; i < 4; i++){
    glDeleteTextures(1, &textureName[i]);
    }

    destroyModelBuffers();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)),
             this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.5F, 0.2F, 0.9F, 0.0F);

    createShaderProgram();
    loadMesh(":/models/sphere.obj", 0);
    loadMesh(":/models/sphere.obj", 1);
    loadMesh(":/models/sphere.obj", 2);
    loadMesh(":/models/cat.obj", 3);
    loadTextures();

    // Initialize transformations.
    updateProjectionTransform();
    updateModelTransforms();

    timer.start(1000.0 / 60.0);
}

void MainView::createShaderProgram() {
    // Create Phong shader program.
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();

    // Get the uniforms for the Phong shader program.
    uniformModelViewTransformPhong  = phongShaderProgram.uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong     = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong            = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong       = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColorPhong          = phongShaderProgram.uniformLocation("lightColor");
    uniformTextureSamplerPhong      = phongShaderProgram.uniformLocation("textureSampler");
}

void MainView::loadMesh(QString name, int i) {
    Model model(name);
    model.unitize();
    QVector<float> meshData = model.getVNTInterleaved();

    meshSize[i] = model.getVertices().size();

    // Generate VAO
    glGenVertexArrays(1, &meshVAO[i]);
    glBindVertexArray(meshVAO[i]);

    // Generate VBO
    glGenBuffers(1, &meshVBO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO[i]);

    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(GL_FLOAT), meshData.data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Set vertex normals to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Set vertex texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MainView::loadTextures() {
    glGenTextures(1, &textureName[0]);
    loadTexture(":/textures/earthmap.png", textureName[0]);

    glGenTextures(1, &textureName[1]);
    loadTexture(":/textures/jupitermap.jpg", textureName[1]);

    glGenTextures(1, &textureName[2]);
    loadTexture(":/textures/mercurymap.jpg", textureName[2]);

    glGenTextures(1, &textureName[3]);
    loadTexture(":/textures/cat_norm.png", textureName[3]);
}

void MainView::loadTexture(QString file, GLuint textureName) {
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, textureName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
}

// --- OpenGL drawing

void MainView::rotateModel(int i, QVector3D coord)
{
    xRotation[i] += coord.x();
    xRotation[i] = fmod(xRotation[i] , 360.0);
    yRotation[i] += coord.y();
    yRotation[i] = fmod(yRotation[i] , 360.0);
    zRotation[i] += coord.z();
    zRotation[i] = fmod(zRotation[i] , 360.0);
}

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    time += 0.1f;
    angle[0] = time * M_PI / (1000 / 60);
    angle[1] = (0.03f * time * M_PI) / (1000 / 60);
    angle[2] = time * M_PI / (1000 / 60);

    //rotate objects
    rotateModel(0, {0.0F, 0.3F, 0.0F});
    rotateModel(1, {0.3F, 0.0F, 0.0F});
    rotateModel(2, {0.0F, 0.6F, 0.0F});
    rotateModel(3, {0.3F, 0.0F, 0.0F});

    updateProjectionTransform();
    updateModelTransforms();

    for(int i = 0; i < 4; i++){
    // Choose the selected shader.
    switch (currentShader) {
    case NORMAL:
        qDebug() << "Normal shader program not implemented";
        break;
    case GOURAUD:
        qDebug() << "Gouraud shader program not implemented";
        break;
    case PHONG:
        phongShaderProgram.bind();
        updatePhongUniforms(i);
        break;
    }

    // Set the texture and draw the mesh.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureName[i]);

    glBindVertexArray(meshVAO[i]);
    glDrawArrays(GL_TRIANGLES, 0, meshSize[i]);
}
    phongShaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    updateProjectionTransform();
}

void MainView::updatePhongUniforms(int i) {
    meshNormalTransform[i] = meshTransform[i].normalMatrix();

    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());
    glUniformMatrix4fv(uniformModelViewTransformPhong, 1, GL_FALSE, meshTransform[i].data());
    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, meshNormalTransform[i].data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3f(uniformLightColorPhong, lightColor.x(), lightColor.y(), lightColor.z());

    glUniform1i(uniformTextureSamplerPhong, 0);
}


void MainView::updateProjectionTransform() {
    float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(60.0F, aspectRatio, 0.01F, 20);
    float n = k * aspectRatio;
    float l = k * 15;
    QVector3D projCoords = QVector3D(15 * sin(l) * cos(n), 0, -(15 * sin(l) * sin(n))).normalized();
    projectionTransform.rotate(-aspectRatio, 0, 0, 1);
    projectionTransform.rotate(70, projCoords.x(), projCoords.y(), projCoords.z());
    projectionTransform.translate(-(15 * sin(l) * sin(n)), -(15 * cos(l)), -(15 * sin(l) * cos(n)));
}

void MainView::updateModelTransforms() {
    for(int i = 0; i < 4; i++){
        meshTransform[i].setToIdentity();
        meshTransform[i].translate(xTranslate[i], yTranslate[i], zTranslate[i]);
        meshTransform[i].rotate(rotation.x(), {1.0F, 0.0F, 0.0F});
        meshTransform[i].rotate(rotation.y(), {0.0F, 1.0F, 0.0F});
        meshTransform[i].rotate(rotation.z(), {0.0F, 0.0F, 1.0F});
        meshTransform[i].rotate(xRotation[i], {1.0F, 0.0F, 0.0F});
        meshTransform[i].rotate(yRotation[i], {0.0F, 1.0F, 0.0F});
        meshTransform[i].rotate(zRotation[i], {0.0F, 0.0F, 1.0F});
    }
    meshTransform[0].scale(scale/50);
    meshTransform[1].scale(scale/100);
    meshTransform[2].scale(scale/100);
    meshTransform[3].scale(scale);

    // variable that determines the step of the orbit
    xTranslate[0] = 0;
    yTranslate[0] = 6;

    xTranslate[1] = sin(angle[0]) * 3 + xTranslate[0];
    yTranslate[1] = cos(angle[0]) * 3 + yTranslate[0];
    zTranslate[1] = zTranslate[0];

    xTranslate[2] = sin(angle[1]) * 3 + xTranslate[0];
    yTranslate[2] = cos(angle[1]) * 3 + yTranslate[0];
    zTranslate[2] = cos(angle[1]) * 3 + zTranslate[0];

    xTranslate[3] = sin(angle[2]) * 3 + xTranslate[0];
    yTranslate[3] = yTranslate[0] + 2;
    zTranslate[3] = cos(angle[2]) * 3 + zTranslate[0];
    update();
}

// --- OpenGL cleanup helpers

void MainView::destroyModelBuffers() {
    for(int i = 0; i < 4; i++){
    glDeleteBuffers(1, &meshVBO[i]);
    glDeleteVertexArrays(1, &meshVAO[i]);
    }
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    rotation = { static_cast<float>(rotateX),
                 static_cast<float>(rotateY),
                 static_cast<float>(rotateZ) };

    updateModelTransforms();
}

void MainView::setScale(int newScale) {
    scale = static_cast<float>(newScale) / 100.0F;
    updateModelTransforms();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    currentShader = shading;
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}



