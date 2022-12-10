#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent), m_camera(glm::vec3(0), glm::vec3(0), glm::vec3(0), 0, 0, 0, 0, 0)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // set clear colour to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // load shader
    m_phong_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert",
                                                       ":/resources/shaders/phong.frag");
    // set up lights
    m_lights = std::vector{SceneLightData(0, glm::vec3(-1,-1,0)),
                           SceneLightData(1, glm::vec3(0,-1,-1))};
    // set up camera
    m_camera = Camera(glm::vec3(7,4,7), glm::vec3(-7,-4,-7), glm::vec3(0,1,0), glm::radians(30.0f),
                      size().width(), size().height(),
                      0.1f, 100.0f);

    // set up shape VBOs
    initializeVBO(m_cubeBuffer, m_cubeVBO, m_cubeVAO, PrimitiveType::PRIMITIVE_CUBE);
    initializeVBO(m_cylinderBuffer, m_cylinderVBO, m_cylinderVAO, PrimitiveType::PRIMITIVE_CYLINDER);
    initializeVBO(m_sphereBuffer, m_sphereVBO, m_sphereVAO, PrimitiveType::PRIMITIVE_SPHERE);

    // sending uniforms to shader
    glUseProgram(m_phong_shader);
    sendGlobalDataToShader(1.0f, 1.0f, 1.0f);
    sendLightsToShader(m_lights);
    glUseProgram(0);

    // set leg segment length
    legSegmentLength = 0.5f;
}

void Realtime::paintGL() {
    // clear screen to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // bind shader
    glUseProgram(m_phong_shader);

    // send camera data to shader
    sendCameraDataToShader(m_camera);

    paintFloor();

    paintLeg(settings.theta1, settings.theta2, settings.theta3);

    // unbind shader
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // modify camera width angle accordingly
    m_camera.changeWidthHeight(size().width(), size().height());

    update(); // asks for a PaintGL() call to occur
}

void Realtime::sceneChanged() {

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // rotate for X
        float thetaX = (float)deltaX / 500.0f;
        m_camera.rotate(glm::vec3(0,1,0), thetaX);

        // rotate for Y
        glm::vec3 sideVec = glm::normalize(glm::cross(glm::normalize(m_camera.look),
                                                      glm::normalize(m_camera.up)));
        float thetaY = (float)deltaY / 500.0f;
        m_camera.rotate(sideVec, thetaY);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    glm::vec3 normLook = glm::normalize(m_camera.look);
    glm::vec3 normRight = glm::normalize(glm::cross(normLook, glm::normalize(m_camera.up)));
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    glm::vec3 deltaLook;
    glm::vec3 deltaRight;
    glm::vec3 deltaWorldUp;
    glm::mat4 translationMat;
    // forward/+look
    if (m_keyMap[Qt::Key_W]) {
        m_camera.move(normLook, deltaTime);
    }
    // backward/-look
    if (m_keyMap[Qt::Key_S]) {
        m_camera.move(-normLook, deltaTime);
    }
    // left/-(look x up)
    if (m_keyMap[Qt::Key_A]) {
        m_camera.move(-normRight, deltaTime);
    }
    // right/(look x up)
    if (m_keyMap[Qt::Key_D]) {
        m_camera.move(normRight, deltaTime);
    }
    // world up/(0,1,0)
    if (m_keyMap[Qt::Key_Space]) {
        m_camera.move(worldUp, deltaTime);
    }
    // world down/(0, -1, 0)
    if (m_keyMap[Qt::Key_Control]) { // ACTUALLY COMMAND KEY
        m_camera.move(-worldUp, deltaTime);
    }

    update(); // asks for a PaintGL() call to occur
}
