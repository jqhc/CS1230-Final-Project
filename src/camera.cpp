#include "glm/fwd.hpp"
#include "glm/gtx/transform.hpp"
#include "utils/scenedata.h"
#include <iostream>
#include "camera.h"

// calculates the width angle given the height angle and dimensions.
float getWidthAngle(float heightAngle, int w, int h) {
    float aspectRatio = (float)w / (float)h;
    return 2*glm::atan(aspectRatio * glm::tan(heightAngle / 2.0f));
}

Camera::Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up,
               float heightAngle,
               int w, int h,
               float near, float far) {
    this->pos = pos;
    this->look = look;
    this->up = up;
    this->heightAngle = heightAngle;
    this->widthAngle = getWidthAngle(heightAngle, w, h);
    this->near = near;
    this->far = far;
}

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

void Camera::changeNearFarPlane(float nearPlane, float farPlane) {
    this->near = nearPlane;
    this->far = farPlane;
}

void Camera::changeWidthHeight(int width, int height) {
    this->widthAngle = getWidthAngle(this->heightAngle, width, height);
}

void Camera::move(glm::vec3 direction, float deltaTime) {
    // calculate translation vector
    glm::vec3 deltaVec = 5.0f * deltaTime * direction;
    // calculate translation matrix
    glm::mat4 translation = glm::translate(deltaVec);
    // move camera position
    this->pos = translation * glm::vec4(pos, 1.0f);
}

void Camera::rotate(glm::vec3 axis, float theta) {
    // calculate rotation matrix about axis
    glm::mat4 rotation = glm::rotate(theta, axis);
    // rotate look and up vectors
    this->look = rotation * glm::vec4(look, 0.0f);
    this->up = rotation * glm::vec4(up, 0.0f);
}
