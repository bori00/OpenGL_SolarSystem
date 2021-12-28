#ifndef Planet_hpp
#define Planet_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "TimedSpaceObject.h"

namespace model_layer {

    class Planet : public TimedSpaceObject
    {
    public:
       
        // the orbit center is considered to be the origin
        Planet(glm::vec3 initial_position, long long rotation_period_seconds, glm::vec3 rotation_axis, long long orbital_period_seconds, glm::vec3 orbit_axis);

        glm::mat4 getSpecificModel(long long current_seconds);

        glm::vec3 getCurrentPosition(long long current_seconds);

    private:
        glm::vec3 initial_position_;
        long long rotation_period_seconds_;
        glm::vec3 rotation_axis_;
        long long orbital_period_seconds_;
        glm::vec3 orbit_axis_;

        float computeRotationAngle(long long current_seconds);

        float computeOrbitAngle(long long current_seconds);
    };

}

#endif /* Planet_hpp */