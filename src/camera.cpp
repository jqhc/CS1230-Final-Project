#include "glm/fwd.hpp"
#include "utils/scenedata.h"
#include <iostream>
#include "camera.h"

// calculates the width angle given the height angle and dimensions.
float getWidthAngle(float heightAngle, int w, int h) {
    float aspectRatio = (float)w / (float)h;
    return 2*glm::atan(aspectRatio * glm::tan(heightAngle / 2.0f));
}

Camera::Camera(const SceneCameraData &cameraData,
               int w, int h,
               float near, float far):
    pos(cameraData.pos), look(cameraData.look), up(cameraData.up),
    heightAngle(cameraData.heightAngle), widthAngle(getWidthAngle(cameraData.heightAngle, w, h)),
    near(near), far(far)
{}

// calculates view matrix
glm::mat4 Camera::viewMatrix() {
    glm::mat4 translationMatrix = glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                                            glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                                            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                                            glm::vec4(-pos.x, -pos.y, -pos.z, 1.0f));

    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - (glm::dot(up, w) * w));
    glm::vec3 u = glm::cross(v, w);

    glm::mat4 rotationMatrix = glm::mat4(glm::vec4(u.x, v.x, w.x, 0.0f),
                                         glm::vec4(u.y, v.y, w.y, 0.0f),
                                         glm::vec4(u.z, v.z, w.z, 0.0f),
                                         glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    return rotationMatrix * translationMatrix;
}

// calculates projection matrix
glm::mat4 Camera::projMatrix() {
    // matrix for mapping from [0, -1] -> [-1, 1] for OpenGL
    glm::mat4 openGLMat(glm::vec4(1, 0, 0, 0),
                        glm::vec4(0, 1, 0, 0),
                        glm::vec4(0, 0, -2, 0),
                        glm::vec4(0, 0, -1, 1));

    // unhinging matrix
    float c = -near / far;
    glm::mat4 unhingeMat(glm::vec4(1, 0, 0, 0),
                         glm::vec4(0, 1, 0, 0),
                         glm::vec4(0, 0, 1.0f/(1.0f+c), -1),
                         glm::vec4(0, 0, -c/(1.0f+c), 0));

    // scaling matrix
    float scaleX = 1.0f/(far * glm::tan(widthAngle / 2));
    float scaleY = 1.0f/(far * glm::tan(heightAngle / 2));
    float scaleZ = 1.0f/far;
    glm::mat4 scaleMat(glm::vec4(scaleX, 0, 0, 0),
                       glm::vec4(0, scaleY, 0, 0),
                       glm::vec4(0, 0, scaleZ, 0),
                       glm::vec4(0, 0, 0, 1));

    return openGLMat * unhingeMat * scaleMat;
}
