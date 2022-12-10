#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

namespace Shapes::Cube {
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
        // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
        glm::vec3 normal = glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight));

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

    inline void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  int param1, std::vector<float>& vertexData) {
        // Task 3: create a single side of the cube out of the 4
        //         given points and makeTile()
        // Note: think about how param 1 affects the number of triangles on
        //       the face of the cube
        glm::vec3 rightStepSize = (topRight - topLeft) / (float)param1;
        glm::vec3 downStepSize = (bottomLeft - topLeft) / (float)param1;
        for (int i = 0; i < param1; i++) {
            for (int j = 0; j < param1; j++) {
                makeTile(topLeft + (float)j*downStepSize + (float)i*rightStepSize,
                         topLeft + (float)j*downStepSize + (float)(i+1)*rightStepSize,
                         topLeft + (float)(j+1)*downStepSize + (float)i*rightStepSize,
                         topLeft + (float)(j+1)*downStepSize + (float)(i+1)*rightStepSize,
                         vertexData);
            }
        }
    }

    inline void makeCube(int param1, int param2, std::vector<float>& vertexData) {
        // Task 4: Use the makeFace() function to make all 6 sides of the cube
        glm::vec3 topBackLeft(-0.5,0.5,-0.5);
        glm::vec3 topBackRight(0.5,0.5,-0.5);
        glm::vec3 topFrontLeft(-0.5,0.5,0.5);
        glm::vec3 topFrontRight(0.5,0.5,0.5);

        glm::vec3 bottomBackLeft(-0.5,-0.5,-0.5);
        glm::vec3 bottomBackRight(0.5,-0.5,-0.5);
        glm::vec3 bottomFrontLeft(-0.5,-0.5,0.5);
        glm::vec3 bottomFrontRight(0.5,-0.5,0.5);

        makeFace(topBackLeft, topBackRight, topFrontLeft, topFrontRight, param1, vertexData);
        makeFace(topFrontLeft, topFrontRight, bottomFrontLeft, bottomFrontRight, param1, vertexData);
        makeFace(bottomFrontLeft, bottomFrontRight, bottomBackLeft, bottomBackRight, param1, vertexData);
        makeFace(bottomBackLeft, bottomBackRight, topBackLeft, topBackRight, param1, vertexData);
        makeFace(topFrontRight, topBackRight, bottomFrontRight, bottomBackRight, param1, vertexData);
        makeFace(topBackLeft, topFrontLeft, bottomBackLeft, bottomFrontLeft, param1, vertexData);
    }
}
