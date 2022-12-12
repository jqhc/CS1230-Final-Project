#ifndef SPIDER_H
#define SPIDER_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class Spider
{
public:
    // constructor for spider class
    Spider(GLuint phong_shader,
           GLuint cylinderVAO, int cylinderBufferSize,
           GLuint sphereVAO, int sphereBufferSize,
           float segLength1, float segLength2,
           float legDiameter);

    //----FIELDS----//
    // GL-related fields (for painting to screen)
    GLuint m_phong_shader;
    GLuint m_cylinderVAO;
    int m_cylinderBufferSize;
    GLuint m_sphereVAO;
    int m_sphereBufferSize;

    // Spider characteristic fields
    float segLength1; // length of first leg segment (closer to ground)
    float segLength2; // length of second leg segment (closer to body)
    float legDiameter; // diameter of all legs

    //----METHODS----//
    // paints spider to screen! main function, to be called in Realtime
    void paintSpider(glm::vec3 bodyPos);

    // paints body of spider
    void paintBody(glm::vec3 bodyPos);

    // paints an individual leg of the spider
    void paintLeg(glm::mat4 legModel,
                  glm::vec3 target);
};

#endif // SPIDER_H
