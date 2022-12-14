#include <iostream>
#include <tuple>
#include <glm/glm.hpp>

namespace IKSolver {
    /**
     * @brief solves the inverse kinematics solution for a leg with two segments.
     *        assume fixed point is at (0,0,0)
     * @param target - the target point (x,y,z) to reach
     * @param segLength1 - the length of the first leg segment (closer to fixed point)
     * @param segLength2 - the length of the second leg segment (closer to target point)
     * @return a tuple containing theta1, theta2, theta3 values of the inverse
     *         kinematics solution.
     */
    inline std::tuple<float, float, float> solveAngles(glm::vec3 target,
                                                       float segLength1, float segLength2) {
        // declare thetas
        float theta1;
        float theta2;
        float theta3;

        // calculate distance from (0,0,0) (fixed point) to target point
        float distToTarget = glm::length(target);
//        std::cout << "distance to target: " << distToTarget << std::endl;

        // if the target point is farther than the length of the entire leg,
        // leg should straighten out and go in that direction.
        if (distToTarget > segLength1 + segLength2) {
            glm::vec3 vecToTarget = glm::normalize(target);
            theta1 = acos(glm::dot(glm::normalize(glm::vec3(vecToTarget.x, 0, vecToTarget.z)),
                                   glm::vec3(1,0,0)));
            if (vecToTarget.z>0) {theta1 = 2.0*M_PI - theta1; }
            theta2 = acos(glm::dot(vecToTarget, glm::vec3(0,1,0)));
            theta3 = M_PI;

        } else {
            // solution is based on triangle with fixed point, joint, and target point
            // as vertices A, B, C respectively. These are the lengths of the sides
            // opposite each vertex.
            float a = segLength2;
            float b = distToTarget;
            float c = segLength1;

            // calculate angles of triangle
            // alpha: angle at vertex A
            float cosAlpha = b!=0 ? (pow(b, 2) + pow(c, 2) - pow(a, 2))/(2.0f*b*c) : 0;
            float alpha = abs(cosAlpha)<=1 ? acos(cosAlpha) : cosAlpha>1 ? 0 : M_PI;
            // gamma: angle between first leg segment and XZ-plane
            float gamma = distToTarget!=0?asin(target.y / distToTarget):0;
            // beta: angle at vertex B
            float cosBeta = (pow(a, 2) + pow(c, 2) - pow(b, 2))/(2.0f*a*c);
            float beta = abs(cosBeta)<=1 ? acos(cosBeta) : cosBeta>1 ? 0 : M_PI;
    //        float aPrime = atan2(target.y, glm::length(glm::vec3(target.x, 0, target.z)));

//            std::cout << "alpha: " << alpha << ", "
//                      << "beta: " << beta << ", "
//                      << "gamma: " << gamma << std::endl;

            theta1 = -atan2(target.z,target.x);
            theta2 = M_PI/2.0f - alpha - gamma;
            theta3 = 2.0f*M_PI-beta;
        }

//        std::cout << "theta1: " << theta1 << ", "
//                  << "theta2: " << theta2 << ", "
//                  << "theta3: " << theta3 << std::endl;

        return std::tuple{theta1, theta2, theta3};
    }
}
