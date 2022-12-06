#ifndef CAMERA_H
#define CAMERA_H
#include "glm/fwd.hpp"
#include "utils/scenedata.h"

class Camera {
public:
    Camera(const SceneCameraData& cameraData,
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


};

#endif // CAMERA_H
