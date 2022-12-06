#pragma once
#include "utils/scenedata.h"
#include "shapes/Shapes.cpp"
#include "settings.h"
#include <GL/glew.h>
#include "realtime.h"
#include <iostream>

// initializes all the VBOs and VAOs for each primitive type
void Realtime::initializeVBO(std::vector<float>& bufferData, GLuint& vbo, GLuint& vao,
                             PrimitiveType type) {
    // populate buffer data
    bufferData = Shapes::getVertexData(settings.shapeParameter1,
                                       settings.shapeParameter2, type);
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

void Realtime::changeBufferData(std::vector<float>& bufferData, GLuint& vbo,
                                PrimitiveType type) {
    // clear and modify buffer data according to new params
    bufferData.clear();
    bufferData = Shapes::getVertexData(settings.shapeParameter1,
                                       settings.shapeParameter2, type);

    // bind corresponding VAO and send data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, bufferData.size()*sizeof(GLfloat),
                 bufferData.data(), GL_STATIC_DRAW);
}

void Realtime::bindCurrVAO(PrimitiveType type) {
    switch(type) {
    case PrimitiveType::PRIMITIVE_CONE:
        glBindVertexArray(m_coneVAO);
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        glBindVertexArray(m_cubeVAO);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        glBindVertexArray(m_cylinderVAO);
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        glBindVertexArray(m_sphereVAO);
        break;
    default:
        throw std::invalid_argument("Not a supported primitive type!");
        break;
    }
}

int Realtime::getBufferSize(PrimitiveType type) {
    switch(type) {
    case PrimitiveType::PRIMITIVE_CONE:
        return coneBuffer.size() / 6;
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        return cubeBuffer.size() / 6;
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        return cylinderBuffer.size() / 6;
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        return sphereBuffer.size() / 6;
        break;
    default:
        throw std::invalid_argument("Not a supported primitive type!");
        break;
    }
}

void Realtime::sendGlobalDataToShader(SceneGlobalData& globalData) {
    glUniform1f(glGetUniformLocation(m_phong_shader, "globalData.ka"), globalData.ka);
    glUniform1f(glGetUniformLocation(m_phong_shader, "globalData.kd"), globalData.kd);
    glUniform1f(glGetUniformLocation(m_phong_shader, "globalData.ks"), globalData.ks);
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

void Realtime::sendMaterialToShader(SceneMaterial& material) {
    glUniform4fv(glGetUniformLocation(m_phong_shader, "objMaterial.cAmbient"),
                 1, &material.cAmbient[0]);
    glUniform4fv(glGetUniformLocation(m_phong_shader, "objMaterial.cDiffuse"),
                 1, &material.cDiffuse[0]);
    glUniform4fv(glGetUniformLocation(m_phong_shader, "objMaterial.cSpecular"),
                 1, &material.cSpecular[0]);

    glUniform1f(glGetUniformLocation(m_phong_shader, "objMaterial.shininess"),
                material.shininess);
}

void Realtime::sendCameraDataToShader() {
    glUniform3fv(glGetUniformLocation(m_phong_shader, "cameraPos"),
                 1, &m_camera.pos[0]);
    // calculate and send projection * view matrix to vertex shader
    glm::mat4 projView = m_camera.projMatrix() * m_camera.viewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "projView"), 1, GL_FALSE, &projView[0][0]);
}

void Realtime::paintAllShapes() {

    // bind phong shader
    glUseProgram(m_phong_shader);

    sendCameraDataToShader();

    for (RenderShapeData &shape : m_renderData.shapes) {
        // send object's material data to shader
        sendMaterialToShader(shape.primitive.material);

        // bind the corresponding VAO
        bindCurrVAO(shape.primitive.type);

        // send model matrix to vertex shader
        glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "model"), 1, GL_FALSE, &shape.ctm[0][0]);
        // calculate and send normal model matrix (i.e. inverse transpose of 3x3 CTM) to vertex shader
        glm::mat3 normModel = glm::inverse(glm::transpose(glm::mat3(shape.ctm)));
        glUniformMatrix3fv(glGetUniformLocation(m_phong_shader, "normModel"), 1, GL_FALSE, &normModel[0][0]);

        // draw VAO
        glDrawArrays(GL_TRIANGLES, 0, getBufferSize(shape.primitive.type));
        // unbind VAO
        glBindVertexArray(0);
    }

    // unbind phong shader
    glUseProgram(0);
}

void Realtime::changeCameraPos(glm::vec3 dir, bool positive, float deltaTime) {
    glm::vec3 deltaVec = (positive?1.0f:-1.0f) * moveSpeed * deltaTime * dir;
    glm::mat4 translationMat(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                             glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                             glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                             glm::vec4(deltaVec.x, deltaVec.y, deltaVec.z, 1.0f));
    m_camera.pos = translationMat * glm::vec4(m_camera.pos, 1.0f);
}

void Realtime::initializeFullscreenQuad() {
    // set fullscreen quad buffer and UV coords
    std::vector<GLfloat> fullscreen_quad_data =
    {-1.0f,  1.0f, 0.0f,
     0.0f, 1.0f,
     -1.0f, -1.0f, 0.0f,
     0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f, 1.0f,
     -1.0f,  1.0f, 0.0f,
     0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,
     1.0f, 0.0f
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // set VAO attributes
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // uv
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat),
                        reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat),
                        reinterpret_cast<void*>(3*sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::makeFBO(int fboWidth, int fboHeight) {
    // TEXTURE
    // generate and bind empty FBO texture
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // set min and mag filter interpolation to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // set UV wrap parameter to clamp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // unbind FBO texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // RENDERBUFFER
    // generate and bind empty FBO renderbuffer
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);

    // unbind renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // FBO
    // generate and bind FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Add texture and renderbuffer as attachments
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // unbind FBO (reset to default FBO)
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::paintFBOTextureToScreen(bool perPixelOn, bool kernelOn,
                                       bool gammaFilterOn, bool sobelFilterOn) {
    // bind texture shader
    glUseProgram(m_texture_shader);

    // set uniform texture variable
    glUniform1i(glGetUniformLocation(m_texture_shader, "fboTexture"), 0);

    // set bool uniforms for filters
    glUniform1i(glGetUniformLocation(m_texture_shader, "perPixelFilterOn"), perPixelOn);
    glUniform1i(glGetUniformLocation(m_texture_shader, "kernelFilterOn"), kernelOn);
    glUniform1i(glGetUniformLocation(m_texture_shader, "gammaFilterOn"), gammaFilterOn);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sobelFilterOn"), sobelFilterOn);

    // send gamma value
    glUniform1f(glGetUniformLocation(m_texture_shader, "gammaVal"), gammaVal);

    // set image width and height for UV sampling purposes
    glUniform1i(glGetUniformLocation(m_texture_shader, "imageWidth"), size().width() * m_devicePixelRatio);
    glUniform1i(glGetUniformLocation(m_texture_shader, "imageHeight"), size().height() * m_devicePixelRatio);

    // set uniform filter kernels
    // SHARPEN
    std::vector<float> sharpenKernel{-1.0f, -1.0f, -1.0f,
                                     -1.0f, 17.0f, -1.0f,
                                     -1.0f, -1.0f, -1.0f};
    for (float& val : sharpenKernel) {
        val /= 9.0f;
    }
    glUniform1fv(glGetUniformLocation(m_texture_shader, "sharpenKernel"), 9, &sharpenKernel[0]);
    // BLUR
    std::vector<float> blurKernel(25, 1.0f);
    for (float& val : blurKernel) {
        val /= 25.0f;
    }
    glUniform1fv(glGetUniformLocation(m_texture_shader, "blurKernel"), 25, &blurKernel[0]);

    // bind fullscreen quad VAO
    glBindVertexArray(m_fullscreen_vao);

    // bind fbo texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    // draw fullscreen quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // unbind texture, VAO and shader
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}







