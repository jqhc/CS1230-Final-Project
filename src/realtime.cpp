#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"

#include <QOpenGLShaderProgram>

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent),
      m_camera(glm::vec3(0), glm::vec3(0), glm::vec3(0), 0, 0, 0, 0, 0),
      m_spider(0, 0, 0, 0, 0, 0, 0, 0, 0)
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

    // load phong shader
    m_phong_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert",
                                                       ":/resources/shaders/phong.frag");

    // set up lights
    m_lights = std::vector{SceneLightData(0, glm::vec3(-1,-1,0)),
                           SceneLightData(1, glm::vec3(0,-1,-1)),
                           SceneLightData(2, glm::vec3(0,1,0))};
    // set up camera
    m_camera = Camera(glm::vec3(7,4,7), glm::vec3(-7,-4,-7), glm::vec3(0,1,0), glm::radians(30.0f),
                      size().width(), size().height(),
                      0.1f, 100.0f);

    // set up shape VBOs
    initializeVBO(m_cubeBuffer, m_cubeVBO, m_cubeVAO, PrimitiveType::PRIMITIVE_CUBE);
    initializeVBO(m_cylinderBuffer, m_cylinderVBO, m_cylinderVAO, PrimitiveType::PRIMITIVE_CYLINDER);
    initializeVBO(m_sphereBuffer, m_sphereVBO, m_sphereVAO, PrimitiveType::PRIMITIVE_SPHERE);

    // sending uniforms to phong shader
    glUseProgram(m_phong_shader);
    sendGlobalDataToShader(m_phong_shader, 1.0f, 1.0f, 1.0f);
    sendLightsToShader(m_phong_shader, m_lights);
    glUseProgram(0);

    // setting up terrain
    m_terrain_shader = new QOpenGLShaderProgram;
    m_terrain_shader->addShaderFromSourceFile(QOpenGLShader::Vertex,":/resources/shaders/terrain.vert");
    m_terrain_shader->addShaderFromSourceFile(QOpenGLShader::Fragment,":/resources/shaders/terrain.frag");

    m_terrainVAO.create();
    m_terrainVAO.bind();

    std::vector<GLfloat> verts = m_terrain.generateTerrain();

    m_terrainVBO.create();
    m_terrainVBO.bind();
    m_terrainVBO.allocate(verts.data(),verts.size()*sizeof(GLfloat));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                             nullptr);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                             reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    m_terrainVBO.release();

    m_terrain_shader->release();

    // setting up spider
    m_spider = Spider(m_phong_shader,
                      m_cylinderVAO, m_cylinderBuffer.size() / 6,
                      m_sphereVAO, m_sphereBuffer.size() / 6,
                      0.4f, 0.4f, 0.05f, 0.2f);
}

void Realtime::paintGL() {
    // clear screen to black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // send camera data to shader
    glUseProgram(m_phong_shader);
    sendCameraDataToShader(m_phong_shader, m_camera);
    glUseProgram(0);

    // paint the ground
    paintFloor(0, 20);

    // paint leg for calculated IK solution angles
//    m_spider.paintLeg(glm::mat4(1), target);
//    // paint the target point as a blue sphere
//    paintTarget(target);

    m_spider.paintSpider();
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // modify camera width angle accordingly
    m_camera.changeWidthHeight(size().width(), size().height());

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
        int deltaX = m_prev_mouse_pos.x - posX;
        int deltaY = m_prev_mouse_pos.y - posY;
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

    // SPIDER MOVEMENT
    if (m_keyMap[Qt::Key_Up]) {
        m_camera.move(m_spider.spiderLook(), deltaTime / 5.0f);
        m_spider.move(deltaTime, true);
    }
    if (m_keyMap[Qt::Key_Down]) {
        m_camera.move(-m_spider.spiderLook(), deltaTime / 5.0f);
        m_spider.move(deltaTime, false);
    }
    if (m_keyMap[Qt::Key_Left]) {
        m_spider.rotateLook(deltaTime, true);
    }
    if (m_keyMap[Qt::Key_Right]) {
        m_spider.rotateLook(deltaTime, false);
    }

    // move time forward for all legs
    for (Leg& leg : m_spider.legs) {
        leg.tick(deltaTime);
    }

    update(); // asks for a PaintGL() call to occur
}
