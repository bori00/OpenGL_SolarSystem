#include "Sun.h"

namespace model_layer {
    Sun::Sun(glm::vec3 initial_position,
        long long rotation_period_seconds,
        glm::vec3 rotation_axis) :
        initial_position_(initial_position),
        rotation_period_seconds_(rotation_period_seconds),
        rotation_axis_(rotation_axis) {}

    glm::mat4 Sun::getSpecificModel(long long current_seconds) {
        glm::mat4 my_model(1.0);

        // translate planet to its current position: 2nd transformation to be applied
        my_model = glm::translate(my_model, initial_position_);

        // rotate planet (rotation): 1st transformation to be applied
        float rotation_angle = computeRotationAngle(current_seconds); // in radians
        my_model = glm::rotate(my_model, rotation_angle, rotation_axis_);

        return my_model;
    }

    glm::vec3 Sun::getCurrentPosition(long long current_seconds) {
        return initial_position_;
    }

    float Sun::computeRotationAngle(long long current_seconds) {
        current_seconds = current_seconds % rotation_period_seconds_;
        float angle = ((float)current_seconds / (float)rotation_period_seconds_) * 2 * glm::pi<float>();;
        return angle;
    }
}
