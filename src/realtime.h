#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "camera.h"
#include "utils/sceneparser.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    bool buffersInitialized = false;

    // render data
    RenderData m_renderData;
    // camera
    Camera m_camera;

    // shape buffers
    std::vector<float> coneBuffer;
    std::vector<float> cubeBuffer;
    std::vector<float> cylinderBuffer;
    std::vector<float> sphereBuffer;
    // shape VBO program IDs
    GLuint m_coneVBO;
    GLuint m_cubeVBO;
    GLuint m_cylinderVBO;
    GLuint m_sphereVBO;
    // shape VAO program IDs
    GLuint m_coneVAO;
    GLuint m_cubeVAO;
    GLuint m_cylinderVAO;
    GLuint m_sphereVAO;

    // shader IDs
    GLuint m_texture_shader;
    GLuint m_phong_shader;

    // fullscreen quad IDs
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    // default FBO ID
    GLuint m_defaultFBO;

    // framebuffer IDs
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    // camera translation speed (in world units)
    float moveSpeed;

    // gamma val for gamma filtering
    float gammaVal;

    // realtime helpers (found in realtime_helpers.cpp)
    void initializeVBO(std::vector<float>& bufferData, GLuint& vbo, GLuint& vao,
                       PrimitiveType type);

    void changeBufferData(std::vector<float>& bufferData, GLuint& vao,
                          PrimitiveType type);

    void bindCurrVAO(PrimitiveType type);

    int getBufferSize(PrimitiveType type);

    void sendGlobalDataToShader(SceneGlobalData& globalData);
    void sendLightsToShader(std::vector<SceneLightData>& lights);

    void sendMaterialToShader(SceneMaterial& material);

    void sendCameraDataToShader();

    void paintAllShapes();

    void changeCameraPos(glm::vec3 dir, bool positive, float deltaTime);

    void initializeFullscreenQuad();

    void makeFBO(int fboWidth, int fboHeight);

    void paintFBOTextureToScreen(bool perPixelOn, bool kernelOn,
                                 bool gammaFilterOn, bool sobelFilterOn);

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;
};
