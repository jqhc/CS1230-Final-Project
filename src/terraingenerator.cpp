#include "terraingenerator.h"

#include <cmath>
#include <tuple>
#include "glm/glm.hpp"

// Constructor
TerrainGenerator::TerrainGenerator()
{
  // Task 8: turn off wireframe shading
  m_wireshade = false; // STENCIL CODE
  // m_wireshade = false; // TA SOLUTION

  // Define resolution of terrain generation
  m_resolution = 500;

  // Generate random vector lookup table
  m_lookupSize = 1024;
  m_randVecLookup.reserve(m_lookupSize);

  // Initialize random number generator
  std::srand(1230);

  // Populate random vector lookup table
  for (int i = 0; i < m_lookupSize; i++)
  {
    m_randVecLookup.push_back(glm::vec2(std::rand() * 1.5 / RAND_MAX - 1.0,
                                        std::rand() * 1.5 / RAND_MAX - 1.0));
   }
}

// Destructor
TerrainGenerator::~TerrainGenerator()
{
    m_randVecLookup.clear();
}

// Helper for generateTerrain()
void addPointToVector(glm::vec3 point, std::vector<float>& vector) {
    vector.push_back(point.x);
    vector.push_back(point.y);
    vector.push_back(point.z);
}

// Generates the geometry of the output triangle mesh
std::vector<float> TerrainGenerator::generateTerrain() {
    std::vector<float> verts;
    verts.reserve(m_resolution * m_resolution * 6);

    for(int x = 0; x < m_resolution; x++) {
        for(int y = 0; y < m_resolution; y++) {
            int x1 = x;
            int y1 = y;

            int x2 = x + 1;
            int y2 = y + 1;

            glm::vec3 p1 = getPosition(x1,y1);
            glm::vec3 p2 = getPosition(x2,y1);
            glm::vec3 p3 = getPosition(x2,y2);
            glm::vec3 p4 = getPosition(x1,y2);

            glm::vec3 n1 = getNormal(x1,y1);
            glm::vec3 n2 = getNormal(x2,y1);
            glm::vec3 n3 = getNormal(x2,y2);
            glm::vec3 n4 = getNormal(x1,y2);

            // tris 1
            // x1y1z1
            // x2y1z2
            // x2y2z3
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p2, verts);
            addPointToVector(n2, verts);
            addPointToVector(getColor(n2, p2), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            // tris 2
            // x1y1z1
            // x2y2z3
            // x1y2z4
            addPointToVector(p1, verts);
            addPointToVector(n1, verts);
            addPointToVector(getColor(n1, p1), verts);

            addPointToVector(p3, verts);
            addPointToVector(n3, verts);
            addPointToVector(getColor(n3, p3), verts);

            addPointToVector(p4, verts);
            addPointToVector(n4, verts);
            addPointToVector(getColor(n4, p4), verts);
        }
    }
    return verts;
}

// Samples the (infinite) random vector grid at (row, col)
glm::vec2 TerrainGenerator::sampleRandomVector(int row, int col)
{
    std::hash<int> intHash;
    int index = intHash(row * 41 + col * 43) % m_lookupSize;
    return m_randVecLookup.at(index);
}

// Takes a grid coordinate (row, col), [0, m_resolution), which describes a vertex in a plane mesh
// Returns a normalized position (x, y, z); x and y in range from [0, 1), and z is obtained from getHeight()
glm::vec3 TerrainGenerator::getPosition(int row, int col) {
    // Normalizing the planar coordinates to a unit square
    // makes scaling independent of sampling resolution.
    float x = 10.0 * ((2.0 * row / m_resolution) - 1.0);
    float y = 10.0 * ((2.0 * col / m_resolution) - 1.0);
    float z = getHeight(x, y);
    return glm::vec3(x,z,-y);
}

// ================== Students, please focus on the code below this point

// Helper for interpolate, ease function
float ease(float x) {
    return (3 * powf(x, 2)) - (2 * powf(x, 3));
}

// Helper for computePerlin() and, possibly, getColor()
float interpolate(float A, float B, float alpha) {
    // Task 4: implement your easing/interpolation function below
    float output = A + ease(alpha)*(B - A);
    // Return 0 as placeholder
    return output;
}

// Takes a normalized (x, y) position, in range [0,1)
// Returns a height value, z, by sampling a noise function
float TerrainGenerator::getHeight(float x, float y) {

    // Task 6: modify this call to produce noise of a different frequency
    float z = 0;
    for (int i = 1; i < 4; ++i) {
        z += computePerlin(x * pow(2, i), y * pow(2, i)) / pow(2, i);
    }

    // Task 7: combine multiple different octaves of noise to produce fractal perlin noise

    // Return 0 as placeholder
    return z;
}

// Computes the normal of a vertex by averaging neighbors
glm::vec3 TerrainGenerator::getNormal(int row, int col) {
    // Task 9: Compute the average normal for the given input indices
    glm::vec3 normal(0, 0, 0);
    glm::vec3 V = getPosition(row, col);

    std::vector<glm::vec3> n{getPosition(row, col + 1), getPosition(row - 1, col + 1),
                                     getPosition(row - 1, col), getPosition(row - 1, col - 1),
                                     getPosition(row, col - 1), getPosition(row + 1, col - 1),
                                     getPosition(row + 1, col), getPosition(row + 1, col + 1)};

    for (int i = 0; i < 8; ++i) {
        normal += glm::normalize(glm::cross(n.at(i) - V, n.at((i+1)%8) - V));
    }

    // Return up as placeholder
    return glm::normalize(normal);
}

// Computes color of vertex using normal and, optionally, position
glm::vec3 TerrainGenerator::getColor(glm::vec3 normal, glm::vec3 position) {
    // Task 10: compute color as a function of the normal and position
    int colorChoice = 3;
    float maxZ = 0.05;
    float epsilon = 0.3;
    glm::vec3 color(1, 1, 1);

    switch (colorChoice) {
    case 0:
    {
        if (position.y > maxZ) {
            color = {1, 1, 1};
        } else {
            color = {0.5, 0.5, 0.5};
        }
        break;
    }
    case 1:
    {
        if (normal.y > 1 - epsilon) {
            color = {1, 1, 1};
        } else {
            color = {0.5, 0.5, 0.5};
        }
        break;
    }
    case 2:
    {
        if (position.y > maxZ && normal.y > 1 - epsilon) {
            color = {1, 1, 1};
        } else {
            color = {0.5, 0.5, 0.5};
        }
        break;
    }
    case 3:
    {
        // ask about the proper way to do this
        if (position.y > maxZ - 0.01) {
            float posZ = interpolate(-0.5, 0.5, position.y);
            float normZ = interpolate(-1, 1, normal.y);
            float colVal = abs(posZ) * abs(normZ) + 0.5;
            colVal = (colVal > 1) ? 1 : colVal;
            colVal = (colVal < 0.5) ? 0.5 : colVal;
            color = {colVal, colVal, colVal};
        } else {
            color = {0.5, 0.5, 0.5};
        }
        break;
    }
    default:
        break;
    }

    // Return white as placeholder
    return color;
}

// Computes the intensity of Perlin noise at some point
float TerrainGenerator::computePerlin(float x, float y) {
    // Task 1: get grid indices (as ints)
    typedef std::vector<std::tuple<int, int>> coord;
    typedef std::vector<glm::vec2> vecs;
    coord gridIndices;
    int flooredX = floor(x);
    int flooredY = floor(y);
    gridIndices.push_back(std::tuple<int, int>(flooredX, flooredY));
    gridIndices.push_back(std::tuple<int, int>(flooredX + 1, flooredY));
    gridIndices.push_back(std::tuple<int, int>(flooredX, flooredY + 1));
    gridIndices.push_back(std::tuple<int, int>(flooredX + 1, flooredY + 1));

    // Task 2: compute offset vectors
    vecs offsetVectors;
    for (auto& i : gridIndices) {
        offsetVectors.push_back(glm::vec2(get<0>(i) - x, get<1>(i) - y));
    }

    // Task 3: compute the dot product between offset and grid vectors
    std::vector<float> dotProducts;
    for (int i = 0; i < offsetVectors.size(); ++i) {
        dotProducts.push_back(glm::dot(offsetVectors.at(i),
                                       sampleRandomVector(get<1>(gridIndices.at(i)), get<0>(gridIndices.at(i)))));
    }

    // Task 5: use your interpolation function to produce the final value
    float G = interpolate(dotProducts.at(0), dotProducts.at(1), x - get<0>(gridIndices.at(0)));
    float H = interpolate(dotProducts.at(2), dotProducts.at(3), x - get<0>(gridIndices.at(2)));
    float result = interpolate(G, H, y - get<1>(gridIndices.at(0)));
    // Return 0 as a placeholder
    return result;
}
