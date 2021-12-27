#ifndef Planet_hpp
#define Planet_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace model {

    class Planet
    {
    public:
       
        Planet(glm::vec3 initial_position, long long rotation_period_seconds);

        glm::mat4 getPlanetSpecificModel(long long current_seconds);

    private:
        glm::vec3 initial_position;
        long long rotation_period_seconds;

        float computeAngle(long long current_seconds);
    };

}

#endif /* Planet_hpp */