#include "mainview.h"

#include <QDateTime>

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
    setModels();
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

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &vbo1);
    glDeleteBuffers(1, &vbo2);
    glDeleteBuffers(1, &vbo3);
    glDeleteVertexArrays(1, &vao1);
    glDeleteVertexArrays(1, &vao2);
    glDeleteVertexArrays(1, &vao3);
    shaderProgram.removeAllShaders();
    shaderProgram.release();
    makeCurrent();
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

    connect(&debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage)),
            this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_ALWAYS);

    // Set the color to be used by glClear. This is, effectively, the background color.
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    vertex A = createVertex(1.0, 1.0, 1.0, 0.0, 0.0, 1.0);
    vertex B = createVertex(1.0, 1.0, -1.0, 1.0, 0.0, 0.0);
    vertex C = createVertex(1.0, -1.0, 1.0, 0.0, 0.0, 1.0);
    vertex D = createVertex(1.0, -1.0, -1.0, 1.0, 0.0, 0.0);
    vertex E = createVertex(-1.0, 1.0, -1.0, 0.0, 0.0, 1.0);
    vertex F = createVertex(-1.0, 1.0, 1.0, 0.0, 1.0, 0.0);
    vertex G = createVertex(-1.0, -1.0, 1.0, 1.0, 0.0, 0.0);
    vertex H = createVertex(-1.0, -1.0, -1.0, 1.0, 0.0, 0.0);

    vertex cube[36] = {
        //front
        A,F,G,G,C,A,

        //right
        C,D,B,
        B,A,C,

        //back
        E,B,D,
        D,H,E,

        //left
        H,G,F,
        F,E,H,

        //down
        C,G,H,H,D,C,

        //up
        B,E,F,F,A,B


                          };

    glGenBuffers(1, &vbo1);
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*36, cube, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),  (GLvoid*)(3 * sizeof(GLfloat)));

    vertex apex = createVertex(0.0, 1.0, 0.0, 1.0, 0.0, 0.0);
    vertex A1 = createVertex(1.0, -1.0, -1.0, 1.0, 0.0, 0.0);
    vertex B1 = createVertex(-1.0, -1.0, 1.0, 0.0, 1.0, 0.0);
    vertex C1 = createVertex(1.0, -1.0, 1.0, 0.0, 0.0, 1.0);
    vertex D1 = createVertex(-1.0, -1.0, -1.0, 1.0, 0.0, 0.0);

    vertex pyramid[18] = {
          //base
          B1,D1,A1,A1,C1,B1,

        //front
        B1,C1,apex,

        D1,B1,apex,

        A1,D1,apex,

        C1,A1,apex


    };

    glGenBuffers(1, &vbo2);
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*18, pyramid, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),  (GLvoid*)(3 * sizeof(GLfloat)));

    Model m(":/models/sphere.obj");
    m.unitize();
    size = m.getVertices().size();
    vertex sphere[size];
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    for(int i = 0; i<size; i++){
        x = (m.getVertices().at(i)).operator[](0)*(0.04);
        y = (m.getVertices().at(i)).operator[](1)*(0.04);
        z = (m.getVertices().at(i)).operator[](2)*(0.04);
        r = (float) rand() / (float) (RAND_MAX);
        g = (float) rand() / (float) (RAND_MAX);
        b = (float) rand() / (float) (RAND_MAX);
        sphere[i] = createVertex(x, y, z, r, g, b);
    }

    glGenBuffers(1, &vbo3);
    glGenVertexArrays(1, &vao3);
    glBindVertexArray(vao3);
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*size, sphere, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),  (GLvoid*)(3 * sizeof(GLfloat)));


    createShaderProgram();
}

void MainView::createShaderProgram() {
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader.glsl");
    shaderProgram.link();

    modelMatrixLocation = shaderProgram.uniformLocation("model_matrix");
    projMatrixLocation = shaderProgram.uniformLocation("projection_matrix");
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram.bind();

    glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, (GLfloat*) projModel->data());

    glBindVertexArray(vao1);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (GLfloat*) cubeModel->data());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(vao2);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (GLfloat*) pyramidModel->data());
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glBindVertexArray(vao3);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (GLfloat*) sphereModel->data());
    glDrawArrays(GL_TRIANGLES, 0, size);


    shaderProgram.release();
}

vertex MainView:: createVertex(float x, float y, float z, float red, float green, float blue){
    vertex vrt;
    vrt.x = x;
    vrt.y = y;
    vrt.z = z;
    vrt.red = red;
    vrt.green = green;
    vrt.blue = blue;

    return vrt;
}

void MainView:: setModels() {
    cubeModel = new QMatrix4x4();
    pyramidModel = new QMatrix4x4();
    cubeModel->translate(2,0,-6);
    pyramidModel->translate(-2,0,-6);
    sphereModel = new QMatrix4x4();
    sphereModel->translate(0,0,-10);
    projModel = new QMatrix4x4();
    projModel->perspective(60.0f, 1.0f, 0.0f, 100.0f);
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
    // TODO: Update projection to fit the new aspect ratio
    projModel->setToIdentity();
    projModel->perspective(60.0f, ((float) newWidth)/((float) newHeight), 0.0f, 100.0f);
//    update();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << "," << rotateZ << ")";
    delete cubeModel;
    delete pyramidModel;
    delete sphereModel;
    setModels();
    cubeModel->rotate(rotateX, 1, 0, 0);
    cubeModel->rotate(rotateY, 0, 1, 0);
    cubeModel->rotate(rotateZ, 0, 0, 1);
    pyramidModel->rotate(rotateX, 1, 0, 0);
    pyramidModel->rotate(rotateY, 0, 1, 0);
    pyramidModel->rotate(rotateZ, 0, 0, 1);
    sphereModel->rotate(rotateX, 1, 0, 0);
    sphereModel->rotate(rotateY, 0, 1, 0);
    sphereModel->rotate(rotateZ, 0, 0, 1);
    update();
}

void MainView::setScale(int scale) {
    qDebug() << "Scale changed to " << scale;
    delete cubeModel;
    delete pyramidModel;
    delete sphereModel;
    setModels();
    float s = (float) scale / (float) 100;
    cubeModel->scale(s);
    pyramidModel->scale(s);
    sphereModel->scale(s);
    update();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
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
