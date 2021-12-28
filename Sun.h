#ifndef Sun_hpp
#define Sun_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "TimedSpaceObject.h"

namespace model_layer {

    class Sun : public TimedSpaceObject
    {
    public:

        Sun(glm::vec3 initial_position, long long rotation_period_seconds, glm::vec3 rotation_axis);

        glm::mat4 getSpecificModel(long long current_seconds);

        glm::vec3 getCurrentPosition(long long current_seconds);

    private:
        glm::vec3 initial_position_;
        long long rotation_period_seconds_;
        glm::vec3 rotation_axis_;

        float computeRotationAngle(long long current_seconds);
    };

}

#endif /* Planet_hpp */