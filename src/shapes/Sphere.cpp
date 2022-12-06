#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

namespace Shapes::Sphere {
    // Inserts a glm::vec3 into a vector of floats.
    // This will come in handy if you want to take advantage of vectors to build your shape!
    inline void insertVec3(std::vector<float> &data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    }

    inline void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  std::vector<float>& vertexData) {
        // Task 5: Implement the makeTile() function for a Sphere
        // Note: this function is very similar to the makeTile() function for Cube,
        //       but the normals are calculated in a different way!
        // triangle 1
        // vertex 1
        insertVec3(vertexData, bottomRight);
        // normal 1
        insertVec3(vertexData, glm::normalize(bottomRight));
        // vertex 2
        insertVec3(vertexData, topRight);
        // normal 2
        insertVec3(vertexData, glm::normalize(topRight));
        // vertex 3
        insertVec3(vertexData, topLeft);
        // normal 3
        insertVec3(vertexData, glm::normalize(topLeft));

        // triangle 2
        // vertex 1
        insertVec3(vertexData, bottomRight);
        // normal 1
        insertVec3(vertexData, glm::normalize(bottomRight));
        // vertex 2
        insertVec3(vertexData, topLeft);
        // normal 2
        insertVec3(vertexData, glm::normalize(topLeft));
        // vertex 3
        insertVec3(vertexData, bottomLeft);
        // normal 3
        insertVec3(vertexData, glm::normalize(bottomLeft));
    }

    inline glm::vec3 sphericalToCartesian(float r, float phi, float theta) {
        return glm::vec3(r*glm::sin(phi)*glm::sin(theta),
                         r*glm::cos(phi),
                         r*glm::sin(phi)*glm::cos(theta));
    }

    inline void makeWedge(float currTheta, float nextTheta, int param1,
                           std::vector<float>& vertexData) {
        // Task 6: create a single wedge of the sphere using the
        //         makeTile() function you implemented in Task 5
        // Note: think about how param 1 comes into play here!

        float phiStep = M_PI / param1;

        for (int i = 0; i < param1; i++) {
            glm::vec3 topLeft = sphericalToCartesian(0.5f, i*phiStep, currTheta);
            glm::vec3 topRight = sphericalToCartesian(0.5f, i*phiStep, nextTheta);
            glm::vec3 bottomLeft = sphericalToCartesian(0.5f, (i+1)*phiStep, currTheta);
            glm::vec3 bottomRight = sphericalToCartesian(0.5f, (i+1)*phiStep, nextTheta);
//            glm::vec3 topLeft(
//                        0.5f*glm::sin((float)i*phiStep)*glm::sin(currentTheta),
//                        0.5f*glm::cos((float)i*phiStep),
//                        0.5f*glm::sin((float)i*phiStep)*glm::cos(currentTheta));
//            glm::vec3 topRight(
//                        0.5f*glm::sin((float)i*phiStep)*glm::sin(nextTheta),
//                        0.5f*glm::cos((float)i*phiStep),
//                        0.5f*glm::sin((float)i*phiStep)*glm::cos(nextTheta));
//            glm::vec3 bottomLeft(
//                        0.5f*glm::sin((float)(i+1)*phiStep)*glm::sin(currentTheta),
//                        0.5f*glm::cos((float)(i+1)*phiStep),
//                        0.5f*glm::sin((float)(i+1)*phiStep)*glm::cos(currentTheta));
//            glm::vec3 bottomRight(
//                        0.5f*glm::sin((float)(i+1)*phiStep)*glm::sin(nextTheta),
//                        0.5f*glm::cos((float)(i+1)*phiStep),
//                        0.5f*glm::sin((float)(i+1)*phiStep)*glm::cos(nextTheta));

            makeTile(topLeft, topRight, bottomLeft, bottomRight, vertexData);
        }
    }

    inline void makeSphere(int param1, int param2, std::vector<float>& vertexData) {
        // Task 7: create a full sphere using the makeWedge() function you
        //         implemented in Task 6
        // Note: think about how param 2 comes into play here!

        float thetaStep = 2 * M_PI / param2;

        for (int i = 0; i < param2; i++) {
            makeWedge((float)i*thetaStep, (float)(i+1)*thetaStep, param1, vertexData);
        }

    }
}
