#include "spider.h"
#include "glm/gtx/transform.hpp"
#include "realtime.h"
#include "ik_solver.cpp"

Spider::Spider(GLuint phong_shader,
               GLuint cylinderVAO, int cylinderBufferSize,
               GLuint sphereVAO, int sphereBufferSize,
               float segLength1, float segLength2,
               float legDiameter)
{
    this->m_phong_shader = phong_shader;
    this->m_cylinderVAO = cylinderVAO;
    this->m_cylinderBufferSize = cylinderBufferSize;
    this->m_sphereVAO = sphereVAO;
    this->m_sphereBufferSize = sphereBufferSize;
    this->segLength1 = segLength1;
    this->segLength2 = segLength2;
    this->legDiameter = legDiameter;
}

void Spider::paintSpider(glm::vec3 bodyPos) {
    //----BODY----//
    paintBody(bodyPos);

    //----LEGS----//
    // left back
    paintLeg(glm::translate(glm::vec3(-0.4f,0,-0.25f)), bodyPos);
    // right back
    paintLeg(glm::translate(glm::vec3(-0.4f,0,0.25f)), bodyPos);

    // left middle
    paintLeg(glm::translate(glm::vec3(0,0,-0.4f)), bodyPos);
    // right middle
    paintLeg(glm::translate(glm::vec3(0,0,0.4f)), bodyPos);

    // left front
    paintLeg(glm::translate(glm::vec3(0.4f,0,-0.25f)), bodyPos);
    // right front
    paintLeg(glm::translate(glm::vec3(0.4f,0,0.25f)), bodyPos);
}

/**
 * @brief Spider::paintBody
 * @param bodyPos
 */
void Spider::paintBody(glm::vec3 bodyPos) {
    glm::mat4 bodyModel = glm::translate(bodyPos) // move it to correct position
            * glm::scale(glm::vec3(0.65f, 0.25f, 0.4f)); // scale to correct size

    SceneMaterial bodyMaterial(glm::vec3(0),
                               glm::vec3(0.4f,0.4f,1.0f),
                               glm::vec3(1,1,1), 5.0f);
    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         bodyMaterial, bodyModel);
}

///**
// * @brief paints a single leg segment to screen
// * @param model - model matrix of segment
// * @param colour - colour (diffuse) of the segment
// */
//void Spider::paintLegSegment(glm::mat4 model, glm::vec3 colour) {
//    // bind phong shader
//    glUseProgram(m_phong_shader);
//    // bind cylinder VAO
//    glBindVertexArray(m_cylinderVAO);

//    // send material uniform
//    Realtime::sendMaterialToShader(m_phong_shader, glm::vec4(0),
//                                   glm::vec4(colour, 0), glm::vec4(1,1,1,0), 10.0f);
//    // send model to vertex shader
//    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "model"), 1, GL_FALSE, &model[0][0]);
//    // calculate and send normal model matrix (i.e. inverse transpose of 3x3 CTM) to vertex shader
//    glm::mat3 normModel = glm::inverse(glm::transpose(glm::mat3(model)));
//    glUniformMatrix3fv(glGetUniformLocation(m_phong_shader, "normModel"), 1, GL_FALSE, &normModel[0][0]);

//    // draw VAO
//    glDrawArrays(GL_TRIANGLES, 0, m_cylinderBufferSize);

//    // unbind VAO
//    glBindVertexArray(0);
//    // unbind phong shader
//    glUseProgram(0);
//}

///**
// * @brief paints the joint ball to screen
// * @param model - model matrix of joint ball
// * @param colour - colour (diffuse) of the joint ball
// */
//void Spider::paintJointBall(glm::vec3 jointPos) {
//    glm::mat4 jointBallModel = glm::translate(jointPos) // move it to joint position
//            * glm::scale(glm::vec3(legDiameter)); // scale to correct size

//    SceneMaterial jointBallMaterial(glm::vec3(0),
//                                    glm::vec3(1.0f,0.4f,0.4f),
//                                    glm::vec3(1,1,1), 10.0f);

//    Realtime::paintShape(m_phong_shader,
//                         m_sphereBufferSize, m_sphereVAO,
//                         jointBallMaterial, jointBallModel);
//}

/**
 * @brief paints a 2-segment leg to the screen, given target position.
 *        uses inverse kinematics to calculate joint angles.
 * @param spiderModel - model matrix of the whole spider
 * @param legModel - model matrix of the individual leg
 * @param target - the target position that the leg should reach (i.e. the body).
 */
void Spider::paintLeg(glm::mat4 legModel,
                      glm::vec3 target) {
    // solve inverse kinematics to get joint angles
    auto [theta1, theta2, theta3] = IKSolver::solveAngles(glm::inverse(legModel)*glm::vec4(target,1), segLength1, segLength2);

    //----SEGMENT 1----//
    // calculate model matrix
    glm::mat4 segModel1 = legModel // move to leg position
            * glm::rotate(theta1, glm::vec3(0,1,0)) // rotate it according to theta1
            * glm::rotate(theta2, glm::vec3(0,0,-1))  // rotate it according to theta2
            * glm::translate(glm::vec3(0, segLength1/2.0f, 0)) // move it up so it's on top of the ground
            * glm::scale(glm::vec3(legDiameter, segLength1, legDiameter)); // scale to correct size

    SceneMaterial segMaterial1(glm::vec3(0),
                               glm::vec3(1.0f,0.4f,0.4f),
                               glm::vec3(1,1,1), 5.0f);

    Realtime::paintShape(m_phong_shader,
                         m_cylinderBufferSize, m_cylinderVAO,
                         segMaterial1, segModel1);

    //----JOINT BALL----//
    // calculate endpoint of segment 1
    glm::vec3 jointPos(segLength1 * glm::sin(theta2) * glm::cos(theta1),
                       segLength1 * glm::cos(theta2),
                       -segLength1 * glm::sin(theta2) * glm::sin(theta1));

    glm::mat4 jointBallModel = legModel // move to leg position
            * glm::translate(jointPos) // move it to joint position
            * glm::scale(glm::vec3(legDiameter)); // scale to correct size

    SceneMaterial jointBallMaterial(glm::vec3(0),
                                    glm::vec3(1.0f,0.4f,0.4f),
                                    glm::vec3(1,1,1), 5.0f);

    Realtime::paintShape(m_phong_shader,
                         m_sphereBufferSize, m_sphereVAO,
                         jointBallMaterial, jointBallModel);

    //----SEGMENT 2----//
    // calculate model matrix
    glm::mat4 segModel2 = legModel // move to leg position
            * glm::translate(jointPos) // move it so that it's on top of segment 1 (at joint position)
            * glm::rotate(theta1, glm::vec3(0,1,0)) // rotate to match theta1 for segment 1
            * glm::rotate(theta3+(float)M_PI+theta2, glm::vec3(0,0,-1)) // rotate for theta3
            * glm::translate(glm::vec3(0, segLength2/2.0f, 0)) // move it up so it's on top of the ground
            * glm::scale(glm::vec3(legDiameter, segLength2, legDiameter)); // scale to correct size

    SceneMaterial segMaterial2(glm::vec3(0),
                               glm::vec3(0.4f,1.0f,0.4f),
                               glm::vec3(1,1,1), 5.0f);

    Realtime::paintShape(m_phong_shader,
                         m_cylinderBufferSize, m_cylinderVAO,
                         segMaterial2, segModel2);

}
