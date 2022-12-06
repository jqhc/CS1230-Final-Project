#include "utils/scenedata.h"
#include <vector>
#include "Cone.cpp"
#include "Cube.cpp"
#include "Cylinder.cpp"
#include "Sphere.cpp"

namespace Shapes {
    inline std::vector<float> getVertexData(int param1, int param2, PrimitiveType type) {
        std::vector<float> vertexData;
        switch(type) {
            case PrimitiveType::PRIMITIVE_CONE:
                // min: param1 - 1, param2 - 3
                param2 = (param2 < 3) ? 3 : param2;
                Cone::makeCone(param1, param2, vertexData);
                break;
            case PrimitiveType::PRIMITIVE_CUBE:
                // min: param1 - 1, param2 - 1
                Cube::makeCube(param1, vertexData);
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                // min: param1 - 1, param2 - 3
                param2 = (param2 < 3) ? 3 : param2;
                Cylinder::makeCylinder(param1, param2, vertexData);
                break;
            case PrimitiveType::PRIMITIVE_SPHERE:
                // min: param1 - 2, param2 - 3
                param1 = (param1 < 2) ? 2 : param1;
                param2 = (param2 < 3) ? 3 : param2;
                Sphere::makeSphere(param1, param2, vertexData);
                break;
            default:
                throw std::invalid_argument("Not a supported primitive type!");
                break;
        }

        return vertexData;
    }
}
