#include "Planet.hpp"

namespace model {

    Planet::Planet(glm::vec3 initial_position, long long rotation_period_seconds) {
        this->initial_position_ = initial_position;
        this->rotation_period_seconds_ = rotation_period_seconds;
    }

    glm::mat4 Planet::getPlanetSpecificModel(long long current_seconds) {
        glm::mat4 my_model;

        // rotate planet
        float angle = computeAngle(current_seconds); // in radians
        my_model = glm::rotate(my_model, angle, glm::vec3(0, 1, 0)); // TODO: change axis...

        // translate planet to its current position
        my_model = glm::translate(my_model, initial_position_);

        return my_model;
    }

    float Planet::computeAngle(long long current_seconds) {
        current_seconds = current_seconds % rotation_period_seconds_;
        float angle = ((float) current_seconds / (float) rotation_period_seconds_) * 2 * glm::pi<float>();;
        return angle;
    }
}