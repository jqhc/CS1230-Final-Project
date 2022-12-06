#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

namespace Shapes::Cone {
    // Inserts a glm::vec3 into a vector of floats.
    // This will come in handy if you want to take advantage of vectors to build your shape!
    inline void insertVec3(std::vector<float> &data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    }

    inline void makeCapTile(glm::vec3 topLeft,
                     glm::vec3 topRight,
                     glm::vec3 bottomLeft,
                     glm::vec3 bottomRight,
                     std::vector<float>& vertexData) {
        glm::vec3 normal(0.0f,-1.0f,0.0f);

        // triangle 1
        // vertex 1
        insertVec3(vertexData, bottomRight);
        // normal 1
        insertVec3(vertexData, normal);
        // vertex 2
        insertVec3(vertexData, topRight);
        // normal 2
        insertVec3(vertexData, normal);
        // vertex 3
        insertVec3(vertexData, topLeft);
        // normal 3
        insertVec3(vertexData, normal);

        // triangle 2
        // vertex 1
        insertVec3(vertexData, bottomRight);
        // normal 1
        insertVec3(vertexData, normal);
        // vertex 2
        insertVec3(vertexData, topLeft);
        // normal 2
        insertVec3(vertexData, normal);
        // vertex 3
        insertVec3(vertexData, bottomLeft);
        // normal 3
        insertVec3(vertexData, normal);
    }

    inline void makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight,
                      bool isTip, glm::vec3 tipNormal,
                      std::vector<float>& vertexData) {
        glm::vec2 leftNormalXZ = glm::normalize(glm::vec2(bottomLeft.x, bottomLeft.z));
        glm::vec2 rightNormalXZ = glm::normalize(glm::vec2(bottomRight.x, bottomRight.z));

        glm::vec3 leftNormal = glm::normalize(glm::vec3(leftNormalXZ.x, 0.5f, leftNormalXZ.y));
        glm::vec3 rightNormal = glm::normalize(glm::vec3(rightNormalXZ.x, 0.5f, rightNormalXZ.y));

        tipNormal.y = rightNormal.y;
        tipNormal = glm::normalize(tipNormal);

        // triangle 1
        // vertex 1
        insertVec3(vertexData, bottomRight);
        // normal 1
        insertVec3(vertexData, rightNormal);
        // vertex 2
        insertVec3(vertexData, topRight);
        // normal 2
        insertVec3(vertexData, isTip?tipNormal:rightNormal);
        // vertex 3
        insertVec3(vertexData, topLeft);
        // normal 3
        insertVec3(vertexData, isTip?tipNormal:leftNormal);

        // triangle 2
        // vertex 1
        insertVec3(vertexData, bottomRight);
        // normal 1
        insertVec3(vertexData, rightNormal);
        // vertex 2
        insertVec3(vertexData, topLeft);
        // normal 2
        insertVec3(vertexData, isTip?tipNormal:leftNormal);
        // vertex 3
        insertVec3(vertexData, bottomLeft);
        // normal 3
        insertVec3(vertexData, leftNormal);
    }

    inline glm::vec3 cylindricalToCartesian(float r, float y, float theta) {
        return glm::vec3(r*glm::sin(theta),
                         y,
                         r*glm::cos(theta));
    }

    inline void makeWedge(float currTheta, float nextTheta, int param1, int param2,
                   std::vector<float>& vertexData) {
        glm::vec3 topLeft;
        glm::vec3 topRight;
        glm::vec3 bottomLeft;
        glm::vec3 bottomRight;
        // top of cylinder
        float rStep = 0.5f / (float)param1;
        float heightStep = 1.0f / (float)param1;
        for (int i = 0; i < param1; i++) {
            // bottom of cone
            topLeft = cylindricalToCartesian(i*rStep, -0.5f, nextTheta);
            topRight = cylindricalToCartesian(i*rStep, -0.5f, currTheta);
            bottomLeft = cylindricalToCartesian((i+1)*rStep, -0.5f, nextTheta);
            bottomRight = cylindricalToCartesian((i+1)*rStep, -0.5f, currTheta);

            makeCapTile(topLeft, topRight, bottomLeft, bottomRight, vertexData);

            // side of cone
            topLeft = cylindricalToCartesian(i*rStep, 0.5f - i*heightStep, currTheta);
            topRight = cylindricalToCartesian(i*rStep, 0.5f - i*heightStep, nextTheta);
            bottomLeft = cylindricalToCartesian((i+1)*rStep, 0.5f - (i+1)*heightStep, currTheta);
            bottomRight = cylindricalToCartesian((i+1)*rStep, 0.5f - (i+1)*heightStep, nextTheta);

            glm::vec3 tipNormal;
            if (i==0) {
                glm::vec2 faceNormalXZ = glm::normalize(glm::vec2((bottomLeft.x+bottomRight.x)/2, (bottomLeft.z+bottomRight.z)/2));
                tipNormal = glm::normalize(glm::vec3(faceNormalXZ.x, 2.0f, faceNormalXZ.y));
            }

            makeSideTile(topLeft, topRight, bottomLeft, bottomRight, (i==0), tipNormal, vertexData);
        }
    }

    inline void makeCone(int param1, int param2, std::vector<float>& vertexData) {
        float thetaStep = 2.0f * M_PI / param2;

        for (int i = 0; i < param2; i++) {
            makeWedge((float)i*thetaStep, (float)(i+1)*thetaStep, param1, param2, vertexData);
        }
    }
}
