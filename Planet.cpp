#include "Planet.hpp"

namespace model_layer {

    Planet::Planet(glm::vec3 initial_position, 
            long long rotation_period_seconds, 
            glm::vec3 rotation_axis, 
            long long orbital_period_seconds, 
            glm::vec3 orbit_axis, 
            float radius) :
                initial_position_(initial_position),
                rotation_period_seconds_(rotation_period_seconds),
                rotation_axis_(rotation_axis),
                orbital_period_seconds_(orbital_period_seconds),
                orbit_axis_(orbit_axis),
                radius_(radius) {}

    glm::mat4 Planet::getSpecificModel(long long current_seconds) {
        glm::mat4 my_model(1.0);

        // rotate planet (on the orbit): 3rd transformation to be applied
        float orbit_angle = computeOrbitAngle(current_seconds);
        my_model = glm::rotate(my_model, orbit_angle, orbit_axis_);
            
        // translate planet to its current position: 2nd transformation to be applied
        my_model = glm::translate(my_model, initial_position_);

        // rotate planet (rotation): 1st transformation to be applied
        float rotation_angle = computeRotationAngle(current_seconds); // in radians
        my_model = glm::rotate(my_model, rotation_angle, rotation_axis_);

        return my_model;
    }

    glm::vec3 Planet::getCurrentPosition(long long current_seconds) {
        glm::mat4 my_model(1.0);

        // rotate planet (on the orbit)
        float orbit_angle = computeOrbitAngle(current_seconds);
        my_model = glm::rotate(my_model, orbit_angle, orbit_axis_);

        return glm::vec3(my_model * glm::vec4(initial_position_, 1.0));
    }

    float Planet::computeRotationAngle(long long current_seconds) {
        current_seconds = current_seconds % rotation_period_seconds_;
        float angle = ((float) current_seconds / (float) rotation_period_seconds_) * 2 * glm::pi<float>();;
        return angle;
    }

    float Planet::computeOrbitAngle(long long current_seconds) {
        current_seconds = current_seconds % orbital_period_seconds_;
        float angle = ((float)current_seconds / (float)orbital_period_seconds_) * 2 * glm::pi<float>();;
        return angle;
    }

    float Planet::getRadius() {
        return radius_;
    }
}