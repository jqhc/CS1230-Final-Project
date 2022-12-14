#ifndef SPIDER_H
#define SPIDER_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "spider/leg.h"

class Spider
{
public:
    // constructor for spider class
    Spider(GLuint phong_shader,
           GLuint cylinderVAO, int cylinderBufferSize,
           GLuint sphereVAO, int sphereBufferSize,
           float segLength1, float segLength2,
           float legDiameter, float spiderHeight);

    //----FIELDS----//
    // GL-related fields (for painting to screen)
    GLuint m_phong_shader;
    GLuint m_cylinderVAO;
    int m_cylinderBufferSize;
    GLuint m_sphereVAO;
    int m_sphereBufferSize;

    // Spider basic characteristic fields
    float segLength1; // length of first leg segment (closer to ground)
    float segLength2; // length of second leg segment (closer to body)
    float legDiameter; // diameter of all legs
    float spiderHeight; // height from ground to center of spider body

    // Spider movement fields
    glm::vec3 pos; // spider position (center of body)
    glm::vec3 look; // vector that spider's looking in
    glm::vec3 up; // up vector of spider
//    glm::mat4 spiderModel;
    glm::mat4 spiderTranslation;
    glm::mat4 spiderRotation;

    // Spider legs
    std::vector<Leg> legs;

    //----METHODS----//
    // paints spider to screen! main function, to be called in Realtime
    void paintSpider();

    // paints body of spider
    void paintBody(glm::mat4 spiderModel);

    // for movement
    void move(float dist, bool forward);
    void rotateLook(float deltaTime, bool right);
};

#endif // SPIDER_H
