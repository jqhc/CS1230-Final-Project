#ifndef LEG_H
#define LEG_H
#include <glm/glm.hpp>
#include <GL/glew.h>

class Leg
{
public:
    // constructor
    Leg(glm::vec3 footPosSpider, glm::vec3 hipPosSpider, glm::vec3 targetPosSpider, glm::mat4 spiderModel,
        float moveTime, float segLength1, float segLength2, float diameter,
        GLuint phong_shader,
        GLuint cylinderVAO, int cylinderBufferSize,
        GLuint sphereVAO, int sphereBufferSize);

    // GL-related fields (for painting to screen)
    GLuint m_phong_shader;
    GLuint m_cylinderVAO;
    int m_cylinderBufferSize;
    GLuint m_sphereVAO;
    int m_sphereBufferSize;

    // BASIC VISUAL CHARACTERISTICS
    float segLength1; // length of first leg segment (near hip)
    float segLength2; // length of second leg segment (near foot)
    float diameter; // diameter of leg

    // POSITIONAL CHARACTERISTICS
    // hip position in spider space. constant
    glm::vec3 hipPosSpider;
    // target position in spider space. constant
    glm::vec3 targetPosSpider;
    // current foot position in world space. constant if not in movestate.
    glm::vec3 currFootPosWorld;
    // spider to world model matrix. updated when spider moves
    glm::mat4 spiderModel;

    // FOR MOVEMENT
    // true if leg is currently in movestate.
    bool moveState;
    // used to store foot position and target and keep them constant
    // during movestate.
    glm::vec3 oldFootPosWorld;
    glm::vec3 oldTargetPosWorld;
    // keeps track of how much time has passed since moveState began.
    // range [0,1]. augmented by deltaTime.
    float timeSinceMove;
    // leg movement animation time (i.e. how long it takes to finish the movement)
    float moveTime;


    // METHODS
    // paints the leg given the current positional characteristics
    void paint();
    // updates the leg's foot position in world space using spider's new model
    void updateSpiderModel(glm::mat4 spiderModel);
    // ticks time forward (only needed while in movestate)
    void tick(float deltaTime);
};

#endif // LEG_H
