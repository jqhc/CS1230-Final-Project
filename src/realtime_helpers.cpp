#pragma once
#include "utils/scenedata.h"
#include "settings.h"
#include <GL/glew.h>
#include <iostream>
#include "realtime.h"
#include "shapes/Cube.cpp"
#include "shapes/Cylinder.cpp"
#include "shapes/Sphere.cpp"

#include <QOpenGLShaderProgram>

/**
 * @brief sends global data params to phong shader
 */
void Realtime::sendTerrainDataToShader(GLuint terrain_shader, Camera& camera) {
    /*
     * uniform mat4 projMatrix;
uniform mat4 mvMatrix;
uniform vec4 cameraPos;
     * */
    glUniformMatrix4fv(glGetUniformLocation(terrain_shader, "projMatrix"), 1, GL_FALSE, &camera.projMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(terrain_shader, "mvMatrix"), 1, GL_FALSE, &camera.viewMatrix()[0][0]);
    glUniform3fv(glGetUniformLocation(terrain_shader, "cameraPos"), 1, &camera.pos[0]);
}

/**
 * @brief sends global data params to phong shader
 */
void Realtime::sendGlobalDataToShader(GLuint phong_shader, float ka, float kd, float ks) {
    glUniform1f(glGetUniformLocation(phong_shader, "globalData.ka"), ka);
    glUniform1f(glGetUniformLocation(phong_shader, "globalData.kd"), kd);
    glUniform1f(glGetUniformLocation(phong_shader, "globalData.ks"), ks);
}

/**
 * @brief sends camera data (position, proj*view matrix) to phong shader
 */
void Realtime::sendCameraDataToShader(GLuint phong_shader, Camera& camera) {
    glUniform3fv(glGetUniformLocation(phong_shader, "cameraPos"),
                 1, &camera.pos[0]);
    // calculate and send projection * view matrix to vertex shader
    glm::mat4 projView = camera.projMatrix() * camera.viewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(phong_shader, "projView"), 1, GL_FALSE, &projView[0][0]);
}

/**
 * @brief send light data to shader. max 16 lights
 */
void Realtime::sendLightsToShader(GLuint phong_shader,std::vector<SceneLightData>& lights) {
    int numLights = lights.size();
    if (numLights > 16) {
        numLights = 16;
    }
    glUniform1i(glGetUniformLocation(phong_shader, "numLights"), numLights);
    for (int i = 0; i < numLights; i++) {
        std::string addr = "lights[" + std::to_string(i) + "]";

        switch(lights[i].type) {
        case LightType::LIGHT_POINT: // 0
            // type
            glUniform1i(glGetUniformLocation(phong_shader, (addr+".lightType").c_str()),
                        0);
            // colour
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".color").c_str()),
                         1, &lights[i].color[0]);
            // attenuation function
            glUniform3fv(glGetUniformLocation(phong_shader, (addr+".function").c_str()),
                         1, &lights[i].function[0]);
            // position
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".pos").c_str()),
                        1, &lights[i].pos[0]);
            break;
        case LightType::LIGHT_DIRECTIONAL: // 1
            // type
            glUniform1i(glGetUniformLocation(phong_shader, (addr+".lightType").c_str()),
                        1);
            // colour
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".color").c_str()),
                         1, &lights[i].color[0]);
            // direction
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".dir").c_str()),
                        1, &lights[i].dir[0]);
            break;
        case LightType::LIGHT_SPOT: // 2
            // type
            glUniform1i(glGetUniformLocation(phong_shader, (addr+".lightType").c_str()),
                        2);
            // colour
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".color").c_str()),
                         1, &lights[i].color[0]);
            // attenuation function
            glUniform3fv(glGetUniformLocation(phong_shader, (addr+".function").c_str()),
                         1, &lights[i].function[0]);
            // position
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".pos").c_str()),
                        1, &lights[i].pos[0]);
            // direction
            glUniform4fv(glGetUniformLocation(phong_shader, (addr+".dir").c_str()),
                        1, &lights[i].dir[0]);
            // penumbra
            glUniform1f(glGetUniformLocation(phong_shader, (addr+".spotPenumbra").c_str()),
                        lights[i].penumbra);
            // angle
            glUniform1f(glGetUniformLocation(phong_shader, (addr+".spotAngle").c_str()),
                        lights[i].angle);
            break;
        default:
            break;
        }
    }
}

/**
 * @brief sends an object's material to phong shader
 */
void Realtime::sendMaterialToShader(GLuint phong_shader,
                                    glm::vec4 cAmbient, glm::vec4 cDiffuse, glm::vec4 cSpecular,
                                    float shininess) {
    glUniform4fv(glGetUniformLocation(phong_shader, "objMaterial.cAmbient"),
                 1, &cAmbient[0]);
    glUniform4fv(glGetUniformLocation(phong_shader, "objMaterial.cDiffuse"),
                 1, &cDiffuse[0]);
    glUniform4fv(glGetUniformLocation(phong_shader, "objMaterial.cSpecular"),
                 1, &cSpecular[0]);

    glUniform1f(glGetUniformLocation(phong_shader, "objMaterial.shininess"),
                shininess);
}

/**
 * @brief sets up the VBO for a given primitive type
 * @param bufferData - reference to vector used to store the vertex/normal data
 * @param vbo - VBO ID in shader
 * @param vao - VAO ID in shader
 * @param type - type of primitive (supports cube, cylinder, sphere)
 */
void Realtime::initializeVBO(std::vector<float>& buffer, GLuint& vbo, GLuint& vao,
                             PrimitiveType type) {
    // populate buffer data
    switch(type) {
    case PrimitiveType::PRIMITIVE_CUBE:
        Shapes::Cube::makeCube(25, 25, buffer);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        Shapes::Cylinder::makeCylinder(25, 25, buffer);
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        Shapes::Sphere::makeSphere(25, 25, buffer);
        break;
    default:
        break;
    }
    // generate and bind VBO ID
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // send buffer data to GPU
    glBufferData(GL_ARRAY_BUFFER, buffer.size()*sizeof(GLfloat),
                 buffer.data(), GL_STATIC_DRAW);
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

void Realtime::paintShape(GLuint shaderID,
                          int bufferSize, GLuint vao,
                          SceneMaterial material, glm::mat4 model) {
    // bind shader
    glUseProgram(shaderID);
    // bind VAO
    glBindVertexArray(vao);

    // send material uniform to shader
    Realtime::sendMaterialToShader(shaderID, material.cAmbient, material.cDiffuse,
                                   material.cSpecular, material.shininess);
    // send model to vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &model[0][0]);
    // calculate and send normal model matrix (i.e. inverse transpose of 3x3 CTM) to vertex shader
    glm::mat3 normModel = glm::inverse(glm::transpose(glm::mat3(model)));
    glUniformMatrix3fv(glGetUniformLocation(shaderID, "normModel"), 1, GL_FALSE, &normModel[0][0]);

    // draw VAO
    glDrawArrays(GL_TRIANGLES, 0, bufferSize);

    // unbind VAO
    glBindVertexArray(0);
    // unbind phong shader
    glUseProgram(0);
}

/**
 * @brief paints the floor to the screen
 * @param height - y-coordinate of the floor (top)
 * @param size - x and z dimension
 */
void Realtime::paintFloor(float y, float size) {
    m_terrain_shader->bind();
    m_terrain_shader->setUniformValue(
                m_terrain_shader->uniformLocation("projMatrix"), QMatrix4x4(glm::value_ptr(m_camera.projMatrix())).transposed());
    m_terrain_shader->setUniformValue(
                m_terrain_shader->uniformLocation("mvMatrix"), QMatrix4x4(glm::value_ptr(m_camera.viewMatrix())).transposed());

    int res = m_terrain.getResolution();


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, res * res * 6);

    m_terrain_shader->release();
}

/**
 * @brief paints the specified target point to the screen
 * @param target - coordinates of target point
 */
void Realtime::paintTarget(glm::vec3 target) {
    glm::mat4 targetModel = glm::translate(target) // move it to joint position
            * glm::scale(glm::vec3(0.09f, 0.09f, 0.09f)); // scale to correct size

    SceneMaterial targetMaterial(glm::vec3(0),
                                 glm::vec3(0.4f,0.4f,1.0f),
                                 glm::vec3(1,1,1), 10.0f);

    paintShape(m_phong_shader,
               m_sphereBuffer.size() / 6, m_sphereVAO,
               targetMaterial, targetModel);
}


