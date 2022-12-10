#pragma once
#include "glm/ext/matrix_transform.hpp"
#include "utils/scenedata.h"
#include "settings.h"
#include <GL/glew.h>
#include <iostream>
#include "realtime.h"
#include "shapes/Cube.cpp"
#include "shapes/Cylinder.cpp"
#include "shapes/Sphere.cpp"

void Realtime::sendGlobalDataToShader(float ka, float kd, float ks) {
    glUniform1f(glGetUniformLocation(m_phong_shader, "globalData.ka"), ka);
    glUniform1f(glGetUniformLocation(m_phong_shader, "globalData.kd"), kd);
    glUniform1f(glGetUniformLocation(m_phong_shader, "globalData.ks"), ks);
}

void Realtime::sendCameraDataToShader(Camera& camera) {
    glUniform3fv(glGetUniformLocation(m_phong_shader, "cameraPos"),
                 1, &camera.pos[0]);
    // calculate and send projection * view matrix to vertex shader
    glm::mat4 projView = camera.projMatrix() * camera.viewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "projView"), 1, GL_FALSE, &projView[0][0]);
}

void Realtime::sendLightsToShader(std::vector<SceneLightData>& lights) {
    int numLights = lights.size();
    if (numLights > 16) {
        numLights = 16;
    }
    glUniform1i(glGetUniformLocation(m_phong_shader, "numLights"), numLights);
    for (int i = 0; i < numLights; i++) {
        std::string addr = "lights[" + std::to_string(i) + "]";

        switch(lights[i].type) {
        case LightType::LIGHT_POINT: // 0
            // type
            glUniform1i(glGetUniformLocation(m_phong_shader, (addr+".lightType").c_str()),
                        0);
            // colour
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".color").c_str()),
                         1, &lights[i].color[0]);
            // attenuation function
            glUniform3fv(glGetUniformLocation(m_phong_shader, (addr+".function").c_str()),
                         1, &lights[i].function[0]);
            // position
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".pos").c_str()),
                        1, &lights[i].pos[0]);
            break;
        case LightType::LIGHT_DIRECTIONAL: // 1
            // type
            glUniform1i(glGetUniformLocation(m_phong_shader, (addr+".lightType").c_str()),
                        1);
            // colour
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".color").c_str()),
                         1, &lights[i].color[0]);
            // direction
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".dir").c_str()),
                        1, &lights[i].dir[0]);
            break;
        case LightType::LIGHT_SPOT: // 2
            // type
            glUniform1i(glGetUniformLocation(m_phong_shader, (addr+".lightType").c_str()),
                        2);
            // colour
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".color").c_str()),
                         1, &lights[i].color[0]);
            // attenuation function
            glUniform3fv(glGetUniformLocation(m_phong_shader, (addr+".function").c_str()),
                         1, &lights[i].function[0]);
            // position
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".pos").c_str()),
                        1, &lights[i].pos[0]);
            // direction
            glUniform4fv(glGetUniformLocation(m_phong_shader, (addr+".dir").c_str()),
                        1, &lights[i].dir[0]);
            // penumbra
            glUniform1f(glGetUniformLocation(m_phong_shader, (addr+".spotPenumbra").c_str()),
                        lights[i].penumbra);
            // angle
            glUniform1f(glGetUniformLocation(m_phong_shader, (addr+".spotAngle").c_str()),
                        lights[i].angle);
            break;
        default:
            break;
        }
    }
}

void Realtime::initializeVBO(std::vector<float>& bufferData, GLuint& vbo, GLuint& vao,
                             PrimitiveType type) {
    // populate buffer data
    switch(type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        Shapes::Cube::makeCube(25, 25, bufferData);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        Shapes::Cylinder::makeCylinder(25, 25, bufferData);
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        Shapes::Sphere::makeSphere(25, 25, bufferData);
        break;
    default:
        break;
    }
    // generate and bind VBO ID
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // send buffer data to GPU
    glBufferData(GL_ARRAY_BUFFER, bufferData.size()*sizeof(GLfloat),
                 bufferData.data(), GL_STATIC_DRAW);
    // generate and bind VAO ID
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // set attributes
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),
                          reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat),
                          reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::sendMaterialToShader(glm::vec4 cAmbient, glm::vec4 cDiffuse, glm::vec4 cSpecular,
                                    float shininess) {
    glUniform4fv(glGetUniformLocation(m_phong_shader, "objMaterial.cAmbient"),
                 1, &cAmbient[0]);
    glUniform4fv(glGetUniformLocation(m_phong_shader, "objMaterial.cDiffuse"),
                 1, &cDiffuse[0]);
    glUniform4fv(glGetUniformLocation(m_phong_shader, "objMaterial.cSpecular"),
                 1, &cSpecular[0]);

    glUniform1f(glGetUniformLocation(m_phong_shader, "objMaterial.shininess"),
                shininess);
}

void Realtime::paintFloor() {
    // bind phong shader
    glUseProgram(m_phong_shader);
    // paint floor
    sendMaterialToShader(glm::vec4(0.0f,0.0f,0.0f,0), glm::vec4(0.5f,0.5f,0.5f,0), glm::vec4(1,1,1,0), 10.0f);
    glBindVertexArray(m_cubeVAO);
    // calculate model matrix
    glm::mat4 floorModel(1);
    floorModel = glm::translate(floorModel, glm::vec3(0,-0.05f,0));
    floorModel = glm::scale(floorModel, glm::vec3(20,0.1f,20));
    // send model matrix to vertex shader
    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "model"), 1, GL_FALSE, &floorModel[0][0]);
    // calculate and send normal model matrix (i.e. inverse transpose of 3x3 CTM) to vertex shader
    glm::mat3 normModel = glm::inverse(glm::transpose(glm::mat3(floorModel)));
    glUniformMatrix3fv(glGetUniformLocation(m_phong_shader, "normModel"), 1, GL_FALSE, &normModel[0][0]);
    // draw VAO
    glDrawArrays(GL_TRIANGLES, 0, m_cubeBuffer.size() / 6);
    // unbind VAO
    glBindVertexArray(0);

    // unbind phong shader
    glUseProgram(0);
}

void Realtime::paintLeg(float theta1, float theta2, float theta3) {
    //----SEGMENT 1----//
    // bind phong shader
    glUseProgram(m_phong_shader);
    // send material uniform
    sendMaterialToShader(glm::vec4(0), glm::vec4(1.0f,0.4f,0.4f,0), glm::vec4(1,1,1,0), 10.0f);
    // bind cylinder VAO
    glBindVertexArray(m_cylinderVAO);
    // calculate and send model matrix
    glm::mat4 legModel1(1);
    // scale to correct size
    legModel1 = glm::scale(glm::vec3(0.08f, legSegmentLength, 0.08f)) * legModel1;
    // move it up so it's on top of the ground
    legModel1 = glm::translate(glm::vec3(0, legSegmentLength/2.0f, 0)) * legModel1;
    // rotate it according to theta1 and theta2
    legModel1 = glm::rotate(theta2, glm::vec3(0,0,-1)) * legModel1;
    legModel1 = glm::rotate(theta1, glm::vec3(0,1,0)) * legModel1;
    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "model"), 1, GL_FALSE, &legModel1[0][0]);
    // calculate and send normal model matrix (i.e. inverse transpose of 3x3 CTM) to vertex shader
    glm::mat3 normModel1 = glm::inverse(glm::transpose(glm::mat3(legModel1)));
    glUniformMatrix3fv(glGetUniformLocation(m_phong_shader, "normModel"), 1, GL_FALSE, &normModel1[0][0]);
    // draw VAO
    glDrawArrays(GL_TRIANGLES, 0, m_cylinderBuffer.size() / 6);

    // calculate endpoint of segment 1
    glm::vec3 jointPos(legSegmentLength * glm::sin(theta2) * glm::cos(theta1),
                       legSegmentLength * glm::cos(theta2),
                       -legSegmentLength * glm::sin(theta2) * glm::sin(theta1));

    //----SEGMENT 2----//
    // bind phong shader
    glUseProgram(m_phong_shader);
    // send material uniform
    sendMaterialToShader(glm::vec4(0), glm::vec4(0.4f,1.0f,0.4f,0), glm::vec4(1,1,1,0), 10.0f);
    // bind cylinder VAO
    glBindVertexArray(m_cylinderVAO);
    // calculate and send model matrix
    glm::mat4 legModel2(1);

    // scale to correct size
    legModel2 = glm::scale(glm::vec3(0.08f, legSegmentLength, 0.08f)) * legModel2;
    // move it up so it's on top of the ground
    legModel2 = glm::translate(glm::vec3(0, legSegmentLength/2.0f, 0)) * legModel2;
    // rotate for theta3
    legModel2 = glm::rotate(theta3+(float)M_PI+theta2, glm::vec3(0,0,-1)) * legModel2;
    // rotate to match theta1 for segment 1
    legModel2 = glm::rotate(theta1, glm::vec3(0,1,0)) * legModel2;
    // move it so that it's on top of segment 1 (at joint position)
    legModel2 = glm::translate(jointPos) * legModel2;

    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "model"), 1, GL_FALSE, &legModel2[0][0]);
    // calculate and send normal model matrix (i.e. inverse transpose of 3x3 CTM) to vertex shader
    glm::mat3 normModel2 = glm::inverse(glm::transpose(glm::mat3(legModel2)));
    glUniformMatrix3fv(glGetUniformLocation(m_phong_shader, "normModel"), 1, GL_FALSE, &normModel2[0][0]);
    // draw VAO
    glDrawArrays(GL_TRIANGLES, 0, m_cylinderBuffer.size() / 6);

    // unbind VAO
    glBindVertexArray(0);

    // unbind phong shader
    glUseProgram(0);
}


