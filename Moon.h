#ifndef Moon_hpp
#define Moon_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "TimedSpaceObject.h"

namespace model_layer {

    class Moon : public TimedSpaceObject
    {
    public:

        // the initial position is taken with respect to the orbit_center
        Moon(glm::vec3 initial_position, long long rotation_period_seconds, glm::vec3 rotation_axis, long long orbital_period_seconds, glm::vec3 orbit_axis, TimedSpaceObject* orbit_center);

        glm::mat4 getSpecificModel(long long current_seconds);

        glm::vec3 getCurrentPosition(long long current_seconds);

    private:
        glm::vec3 initial_position_;
        long long rotation_period_seconds_;
        glm::vec3 rotation_axis_;
        long long orbital_period_seconds_;
        glm::vec3 orbit_axis_;
        model_layer::TimedSpaceObject* orbit_center_;

        float computeRotationAngle(long long current_seconds);

        float computeOrbitAngle(long long current_seconds);
    };

}

#endif /* Moon_hpp */