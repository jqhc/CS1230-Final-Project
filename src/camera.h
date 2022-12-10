#ifndef CAMERA_H
#define CAMERA_H
#include "glm/fwd.hpp"
#include "utils/scenedata.h"

class Camera {
public:
    Camera(glm::vec3 pos, glm::vec3 look, glm::vec3 up,
           float heightAngle,
           int w, int h,
           float near, float far);

    glm::vec3 pos;
    glm::vec3 look;
    glm::vec3 up;

    float heightAngle;
    float widthAngle;

    float near;
    float far;

    glm::mat4 viewMatrix();
    glm::mat4 projMatrix();

    void changeNearFarPlane(float nearPlane, float farPlane);

    void changeWidthHeight(int width, int height);

    void move(glm::vec3 direction, float deltaTime);

    void rotate(glm::vec3 axis, float theta);
};

#endif // CAMERA_H
