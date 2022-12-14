#include "leg.h"
#include "glm/gtx/transform.hpp"
#include "spider/ik_solver.cpp"
#include "utils/scenedata.h"
#include "realtime.h"

Leg::Leg(glm::vec3 footPosSpider, glm::vec3 hipPosSpider, glm::vec3 targetPosSpider,
         glm::mat4 spiderModel,
         float moveTime, float segLength1, float segLength2, float diameter,
         GLuint phong_shader,
         GLuint cylinderVAO, int cylinderBufferSize,
         GLuint sphereVAO, int sphereBufferSize) {
    // set GL characteristics
    this->m_phong_shader = phong_shader;
    this->m_cylinderVAO = cylinderVAO;
    this->m_cylinderBufferSize = cylinderBufferSize;
    this->m_sphereVAO = sphereVAO;
    this->m_sphereBufferSize = sphereBufferSize;

    // set basic characteristics
    this->segLength1 = segLength1;
    this->segLength2 = segLength2;
    this->diameter = diameter;

    // set foot positions. initially old = current
    this->currFootPosWorld = spiderModel * glm::vec4(footPosSpider,1);
    this->oldFootPosWorld = this->currFootPosWorld;

    // spider to world model matrix
    this->spiderModel = spiderModel;

    // setting constant position fields
    this->targetPosSpider = targetPosSpider;//glm::vec3(0, footPosSpider.y, footPosSpider.z);
    this->oldTargetPosWorld = spiderModel * glm::vec4(this->targetPosSpider, 1);
    this->hipPosSpider = hipPosSpider;

    // set movement fields to default
    this->moveState = false;
    this->timeSinceMove = 0;
    this->moveTime = moveTime;
}

void Leg::updateSpiderModel(glm::mat4 spiderModel) {
    // update spider model
    this->spiderModel = spiderModel;

    // calculate new target position
    glm::vec3 targetPosWorld = spiderModel * glm::vec4(targetPosSpider,1);

    // if foot is too far from target, and not already in movestate, initiate movestate
    if (!moveState && glm::distance(this->currFootPosWorld, targetPosWorld) > 0.5f) {
        // save current foot position and current target position
        oldFootPosWorld = currFootPosWorld;
        oldTargetPosWorld = targetPosWorld + 0.3f*(targetPosWorld - currFootPosWorld);

        // initiate movestate
        moveState = true;
        timeSinceMove = 0.0f;
    }

    // if currently in movestate
    if (moveState) {
        // if full move time has elapsed, end movestate and reset
        if (timeSinceMove >= 1.0f) {
            this->currFootPosWorld = oldTargetPosWorld;
            moveState = false;
            timeSinceMove = 0.0f;
        // if still in movestate, move foot to interpolated position
        } else {
            // interpolate position between saved foot and target positions
            this->currFootPosWorld = (1.0f-timeSinceMove) * oldFootPosWorld + timeSinceMove * oldTargetPosWorld;
            // add sin function to Y value to emulate picking up and putting down foot
            this->currFootPosWorld.y += sin(timeSinceMove * M_PI) / 4.0f;
        }
    }
}

void Leg::tick(float deltaTime) {
    if (moveState) {
        timeSinceMove += deltaTime / moveTime;
    }
}

/**
 * @brief paints the leg to the screen given the current state.
 *        depends on current foot position in world space.
 */
void Leg::paint() {
    // calculate "leg model" which translates from leg space to world space
    glm::mat4 legToWorld = glm::translate(currFootPosWorld);

    // solve inverse kinematics to get joint angles.
    // we multiply the hip position by the spider model to get to world space, and then
    // by the inverse leg matrix to get back to leg space.
    auto [theta1, theta2, theta3] = IKSolver::solveAngles(glm::inverse(legToWorld)*spiderModel*glm::vec4(hipPosSpider,1),
                                                          segLength1, segLength2);

    //----SEGMENT 1----//
    // calculate model matrix
    glm::mat4 segModel1 = legToWorld // move to spider space
            * glm::rotate(theta1, glm::vec3(0,1,0)) // rotate it according to theta1
            * glm::rotate(theta2, glm::vec3(0,0,-1))  // rotate it according to theta2
            * glm::translate(glm::vec3(0, segLength1/2.0f, 0)) // move it up so it's on top of the ground
            * glm::scale(glm::vec3(diameter, segLength1, diameter)); // scale to correct size

    SceneMaterial segMaterial1(glm::vec3(0),
                               glm::vec3(0.0f,0.0f,0.0f),
                               glm::vec3(1,1,1), 5.0f);

    Realtime::paintShape(m_phong_shader,
                         m_cylinderBufferSize, m_cylinderVAO,
                         segMaterial1, segModel1);

    //----JOINT BALL----//
    // calculate endpoint of segment 1
    glm::vec3 jointPos(segLength1 * glm::sin(theta2) * glm::cos(theta1),
                       segLength1 * glm::cos(theta2),
                       -segLength1 * glm::sin(theta2) * glm::sin(theta1));

    glm::mat4 jointBallModel = legToWorld // move to spider space
            * glm::translate(jointPos) // move it to joint position
            * glm::scale(glm::vec3(diameter)); // scale to correct size

    SceneMaterial jointBallMaterial(glm::vec3(0),
                                    glm::vec3(0.0f,0.0f,0.0f),
                                    glm::vec3(1,1,1), 5.0f);

    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         jointBallMaterial, jointBallModel);

    //----SEGMENT 2----//
    // calculate model matrix
    glm::mat4 segModel2 = legToWorld // move to spider space
            * glm::translate(jointPos) // move it so that it's on top of segment 1 (at joint position)
            * glm::rotate(theta1, glm::vec3(0,1,0)) // rotate to match theta1 for segment 1
            * glm::rotate(theta3+(float)M_PI+theta2, glm::vec3(0,0,-1)) // rotate for theta3
            * glm::translate(glm::vec3(0, segLength2/2.0f, 0)) // move it up so it's on top of the ground
            * glm::scale(glm::vec3(diameter, segLength2, diameter)); // scale to correct size

    SceneMaterial segMaterial2(glm::vec3(0),
                               glm::vec3(0.0f,0.0f,0.0f),
                               glm::vec3(1,1,1), 5.0f);

    Realtime::paintShape(m_phong_shader,
                         m_cylinderBufferSize, m_cylinderVAO,
                         segMaterial2, segModel2);
}
