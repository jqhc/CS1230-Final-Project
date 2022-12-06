#include "realtime.h"
#pragma once

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include "utils/sceneparser.h"
#include "shapes/Shapes.cpp"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent), m_camera(m_renderData.cameraData, 0, 0, 0, 0)
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

    // clean up VBO and VAO memory
    std::vector<GLuint> vbos{m_coneVBO, m_cubeVBO, m_cylinderVBO, m_sphereVBO};
    std::vector<GLuint> vaos{m_coneVAO, m_cubeVAO, m_cylinderVAO, m_sphereVAO};
    glDeleteBuffers(4, vbos.data());
    glDeleteVertexArrays(4, vaos.data());

    // delete shader data
    glDeleteProgram(m_phong_shader);
    glDeleteProgram(m_texture_shader);

    // delete fullscreen quad data
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    // delete FBO data
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    doneCurrent();

    this->doneCurrent();
}

void Realtime::initializeGL() {
    // set camera move speed in world units
    moveSpeed = 5.0f;
    // set default FBO ID
    m_defaultFBO = 2;
    // set gamma value
    gammaVal = 1.5f;

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

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // set default clear colour to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Task 1: call ShaderLoader::createShaderProgram with the paths to the vertex
    //         and fragment shaders. Then, store its return value in `m_shader`
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert",
                                                         ":/resources/shaders/texture.frag");
    m_phong_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert",
                                                       ":/resources/shaders/phong.frag");

    // initializing buffer data
    // default param1 = 5, param2 = 5
    initializeVBO(coneBuffer, m_coneVBO, m_coneVAO, PrimitiveType::PRIMITIVE_CONE);
    initializeVBO(cubeBuffer, m_cubeVBO, m_cubeVAO, PrimitiveType::PRIMITIVE_CUBE);
    initializeVBO(cylinderBuffer, m_cylinderVBO, m_cylinderVAO, PrimitiveType::PRIMITIVE_CYLINDER);
    initializeVBO(sphereBuffer, m_sphereVBO, m_sphereVAO, PrimitiveType::PRIMITIVE_SPHERE);
    buffersInitialized = true;

    // initializing fullscreen quad data
    initializeFullscreenQuad();

    // initializing the FBO
    makeFBO(size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    // send (updated) camera data to shader

    // FBO STUFF
    // bind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // call glViewport
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // clear FBO
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw all shapes to FBO
    paintAllShapes();

    // FULLSCREEN QUAD STUFF
    // bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    // call glViewport
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // clear default framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // paint FBO texture data to screen
    paintFBOTextureToScreen(settings.perPixelFilter, settings.kernelBasedFilter,
                            settings.extraCredit1, settings.extraCredit2);
}

void Realtime::resizeGL(int w, int h) {
    std::cout << "window resized!" << std::endl;
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    // modify camera for new screen size
    m_camera = Camera(m_renderData.cameraData, size().width(), size().height(),
                      settings.nearPlane, settings.farPlane);

    // delete and regenerate FBO
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    makeFBO(size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    update();
}

void Realtime::sceneChanged() {
    // flags for debugging
    std::cout << "scene changed! filepath: " << settings.sceneFilePath << std::endl;

    // changing render data to match new scene file
    RenderData metaData;
    if (!SceneParser::parse(settings.sceneFilePath, metaData)) {
        throw std::invalid_argument("Couldn't read file!");
    }
    m_renderData = metaData;

    // modify camera for new scene file
    m_camera = Camera(m_renderData.cameraData, size().width(), size().height(),
                      settings.nearPlane, settings.farPlane);

    glUseProgram(m_phong_shader);
    sendGlobalDataToShader(metaData.globalData);
    std::cout << "global data sent!" << std::endl;
    sendLightsToShader(metaData.lights);
    std::cout << "light data sent!" << std::endl;
    glUseProgram(0);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    // recalculate and send buffer data for given params
    if (buffersInitialized) {
        std::cout << "buffers changed!" << std::endl;
        changeBufferData(coneBuffer, m_coneVBO, PrimitiveType::PRIMITIVE_CONE);
        changeBufferData(cubeBuffer, m_cubeVBO, PrimitiveType::PRIMITIVE_CUBE);
        changeBufferData(cylinderBuffer, m_cylinderVBO, PrimitiveType::PRIMITIVE_CYLINDER);
        changeBufferData(sphereBuffer, m_sphereVBO, PrimitiveType::PRIMITIVE_SPHERE);
    }

    // modify camera for new near and far plane settings
    m_camera = Camera(m_renderData.cameraData, size().width(), size().height(),
                      settings.nearPlane, settings.farPlane);

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

        // Use deltaX and deltaY here to rotate
        float thetaX = (float)deltaX / 500.0f;
        glm::mat4 rotationX(glm::vec4(glm::cos(thetaX), 0.0f, -glm::sin(thetaX), 0.0f),
                            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                            glm::vec4(glm::sin(thetaX), 0.0f, glm::cos(thetaX), 0.0f),
                            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::vec3 sideVec = glm::normalize(glm::cross(glm::normalize(m_camera.look),
                                                      glm::normalize(m_camera.up)));
        float thetaY = (float)deltaY / 500.0f;
        glm::mat4 rotationY(glm::vec4(glm::cos(thetaY) + std::pow(sideVec.x, 2)*(1-glm::cos(thetaY)),
                                      sideVec.x*sideVec.y*(1-glm::cos(thetaY)) + sideVec.z*glm::sin(thetaY),
                                      sideVec.x*sideVec.z*(1-glm::cos(thetaY)) - sideVec.y*glm::sin(thetaY),
                                      0.0f),
                            glm::vec4(sideVec.x*sideVec.y*(1-glm::cos(thetaY)) - sideVec.z*glm::sin(thetaY),
                                      glm::cos(thetaY) + std::pow(sideVec.y, 2)*(1-glm::cos(thetaY)),
                                      sideVec.y*sideVec.z*(1-glm::cos(thetaY)) + sideVec.x*glm::sin(thetaY),
                                      0.0f),
                            glm::vec4(sideVec.x*sideVec.z*(1-glm::cos(thetaY)) + sideVec.y*glm::sin(thetaY),
                                      sideVec.y*sideVec.z*(1-glm::cos(thetaY)) - sideVec.x*glm::sin(thetaY),
                                      glm::cos(thetaY) + std::pow(sideVec.z, 2)*(1-glm::cos(thetaY)),
                                      0.0f),
                            glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        m_camera.look = rotationX * rotationY * glm::vec4(m_camera.look, 0.0f);
        m_camera.up = rotationX * rotationY * glm::vec4(m_camera.up, 0.0f);

        m_renderData.cameraData.look = glm::vec4(m_camera.look, 0.0f);
        m_renderData.cameraData.up = glm::vec4(m_camera.up, 0.0f);

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
        changeCameraPos(normLook, true, deltaTime);
    }
    // backward/-look
    if (m_keyMap[Qt::Key_S]) {
        changeCameraPos(normLook, false, deltaTime);
    }
    // left/-(look x up)
    if (m_keyMap[Qt::Key_A]) {
        changeCameraPos(normRight, false, deltaTime);
    }
    // right/(look x up)
    if (m_keyMap[Qt::Key_D]) {
        changeCameraPos(normRight, true, deltaTime);
    }
    // world up/(0,1,0)
    if (m_keyMap[Qt::Key_Space]) {
        changeCameraPos(worldUp, true, deltaTime);
    }
    // world down/(0, -1, 0)
    if (m_keyMap[Qt::Key_Control]) { // ACTUALLY COMMAND KEY
        changeCameraPos(worldUp, false, deltaTime);
    }
    // set render data's pos
    m_renderData.cameraData.pos = glm::vec4(m_camera.pos, 1.0f);

    update(); // asks for a PaintGL() call to occur
}
