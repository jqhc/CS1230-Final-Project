#include "spider.h"
#include "glm/gtx/transform.hpp"
#include "realtime.h"
#include "ik_solver.cpp"

Spider::Spider(GLuint phong_shader,
               GLuint cylinderVAO, int cylinderBufferSize,
               GLuint sphereVAO, int sphereBufferSize,
               float segLength1, float segLength2,
               float legDiameter, float spiderHeight)
{
    this->m_phong_shader = phong_shader;
    this->m_cylinderVAO = cylinderVAO;
    this->m_cylinderBufferSize = cylinderBufferSize;
    this->m_sphereVAO = sphereVAO;
    this->m_sphereBufferSize = sphereBufferSize;

    this->segLength1 = segLength1;
    this->segLength2 = segLength2;
    this->legDiameter = legDiameter;
    this->spiderHeight = spiderHeight;

    this->pos = glm::vec3(0, spiderHeight, 0);
    this->look = glm::vec3(1,0,0);
    this->up = glm::vec3(0,1,0);
    this->spiderTranslation = glm::translate(pos);
    this->spiderRotation = glm::mat4(1);

    this->legs = std::vector<Leg>{};
    // back left
    legs.push_back(Leg(glm::vec3(-0.4f,-spiderHeight,-0.25f), glm::vec3(-0.2f,0,-0.15f), glm::vec3(0.0f,-spiderHeight,-0.25f),
                       this->spiderTranslation,
                       0.2f, segLength1, segLength2, legDiameter,
                       phong_shader, cylinderVAO, cylinderBufferSize, sphereVAO, sphereBufferSize));
    // back right
    legs.push_back(Leg(glm::vec3(-0.4f,-spiderHeight,0.25f), glm::vec3(-0.2f,0,0.15f), glm::vec3(-0.3f,-spiderHeight,0.25f),
                       this->spiderTranslation,
                       0.2f, segLength1, segLength2, legDiameter,
                       phong_shader, cylinderVAO, cylinderBufferSize, sphereVAO, sphereBufferSize));
    // middle left
    legs.push_back(Leg(glm::vec3(0,-spiderHeight,-0.4f), glm::vec3(0,0,-0.2f), glm::vec3(0.3f,-spiderHeight,-0.4f),
                       this->spiderTranslation,
                       0.2f, segLength1, segLength2, legDiameter,
                       phong_shader, cylinderVAO, cylinderBufferSize, sphereVAO, sphereBufferSize));
    // middle right
    legs.push_back(Leg(glm::vec3(0,-spiderHeight,0.4f), glm::vec3(0,0,0.2f), glm::vec3(0.0f,-spiderHeight,0.4f),
                       this->spiderTranslation,
                       0.2f, segLength1, segLength2, legDiameter,
                       phong_shader, cylinderVAO, cylinderBufferSize, sphereVAO, sphereBufferSize));
    // front left
    legs.push_back(Leg(glm::vec3(0.4f,-spiderHeight,-0.25f), glm::vec3(0.2f,0,-0.15f), glm::vec3(0.6f,-spiderHeight,-0.25f),
                       this->spiderTranslation,
                       0.2f, segLength1, segLength2, legDiameter,
                       phong_shader, cylinderVAO, cylinderBufferSize, sphereVAO, sphereBufferSize));
    // back right
    legs.push_back(Leg(glm::vec3(0.4f,-spiderHeight,0.25f), glm::vec3(0.2f,0,0.15f), glm::vec3(0.3f,-spiderHeight,0.25f),
                       this->spiderTranslation,
                       0.2f, segLength1, segLength2, legDiameter,
                       phong_shader, cylinderVAO, cylinderBufferSize, sphereVAO, sphereBufferSize));
}

/**
 * @brief moves the spider in the direction of look.
 * @param deltaTime
 * @param forward
 */
void Spider::move(float deltaTime, bool forward) {
    // calculate vector of movement direction and magnitude
    glm::vec3 deltaVec = 1.0f * deltaTime * look;
    // flip sign if moving backwards
    if (!forward) deltaVec = -deltaVec;
    // modify spider position
    pos += deltaVec;
    // modify spider model (translates body parts)
    spiderTranslation *= glm::translate(deltaVec);
}

/**
 * @brief rotates the direction the spider left and right, about the up vector.
 * @param deltaTime - the amount of time the "rotate" button was pressed
 * @param right - true if rotating right, false if rotating left
 */
void Spider::rotateLook(float deltaTime, bool right) {
    // calculate angle of rotation
    float theta = deltaTime * 2.0f;
    // flip sign if rotating to the left
    if (!right) theta = -theta;
    // rotate look vector
    look = glm::rotate(theta, up) * glm::vec4(look, 0);
    // modify spider model for rotation
    spiderRotation *= glm::rotate(theta, up);
}

/**
 * @brief paints the entire spider!
 */
void Spider::paintSpider() {
    glm::mat4 spiderModel = spiderTranslation * spiderRotation;

    //----BODY----//
    paintBody(spiderModel);

    for (Leg& leg : this->legs) {
        leg.updateSpiderModel(spiderModel);
        leg.paint();
    }
}

/**
 * @brief paints body of spider.
 * @param spiderModel - model matrix of spider which translates from world space
 *                      to spider space.
 */
void Spider::paintBody(glm::mat4 spiderModel) {
    // paint spider body
    glm::mat4 bodyModel = spiderModel // move to world space with spider model matrix
            * glm::scale(glm::vec3(0.65f, 0.25f, 0.4f)); // scale to correct size

    SceneMaterial bodyMaterial(glm::vec3(0),
                               glm::vec3(0.0f,0.0f,0.0f),
                               glm::vec3(1,1,1), 5.0f);
    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         bodyMaterial, bodyModel);

    // paint eyes
    SceneMaterial eyeMaterial(glm::vec3(0),
                              glm::vec3(1.0f,1.0f,1.0f),
                              glm::vec3(1,1,1), 10.0f);

    glm::mat4 leftEyeModel = spiderModel
            * glm::translate(glm::vec3(0.3f, 0.03f, -0.1f))
            * glm::scale(glm::vec3(0.1f,0.1f,0.1f));
    glm::mat4 leftPupilModel = spiderModel
            * glm::translate(glm::vec3(0.33f, 0.03f, -0.1f))
            * glm::scale(glm::vec3(0.05f,0.05f,0.05f));

    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         eyeMaterial, leftEyeModel);
    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         bodyMaterial, leftPupilModel);


    glm::mat4 rightEyeModel = spiderModel
            * glm::translate(glm::vec3(0.3f, 0.03f, 0.1f))
            * glm::scale(glm::vec3(0.1f,0.1f,0.1f));
    glm::mat4 rightPupilModel = spiderModel
            * glm::translate(glm::vec3(0.33f, 0.03f, 0.1f))
            * glm::scale(glm::vec3(0.05f,0.05f,0.05f));

    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         eyeMaterial, rightEyeModel);
    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         bodyMaterial, rightPupilModel);
}
