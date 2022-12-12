#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "camera.h"
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
#include "spider.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void settingsChanged();

    // helpers for sending uniforms to shader
    // USED BY SPIDER
    static void sendGlobalDataToShader(GLuint phong_shader, float ka, float kd, float ks);
    static void sendCameraDataToShader(GLuint phong_shader, Camera& camera);
    static void sendLightsToShader(GLuint phong_shader, std::vector<SceneLightData>& lights);
    static void sendMaterialToShader(GLuint phong_shader,
                                     glm::vec4 cAmbient, glm::vec4 cDiffuse, glm::vec4 cSpecular,
                                     float shininess);

    // paints a shape
    static void paintShape(GLuint shaderID,
                           int bufferSize, GLuint vao,
                           SceneMaterial material, glm::mat4 model);

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

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // shader ID
    GLuint m_phong_shader;

    // camera
    Camera m_camera;

    // lights
    std::vector<SceneLightData> m_lights;

    // shape buffers
    std::vector<float> m_cubeBuffer;
    std::vector<float> m_cylinderBuffer;
    std::vector<float> m_sphereBuffer;
    // shape VBO program IDs
    GLuint m_cubeVBO;
    GLuint m_cylinderVBO;
    GLuint m_sphereVBO;
    // shape VAO program IDs
    GLuint m_cubeVAO;
    GLuint m_cylinderVAO;
    GLuint m_sphereVAO;

    // spider object
    Spider m_spider;

    // paints floor to screen
    void paintFloor(float y, float size);

    // paints target point
    void paintTarget(glm::vec3 target);

    // helper for initializing the shape VBOs
    void initializeVBO(std::vector<float>& buffer, GLuint& vbo, GLuint& vao,
                       PrimitiveType type);
};
