#include "moon.h"

namespace model_layer {

    Moon::Moon(glm::vec3 initial_position,
        long long rotation_period_seconds,
        glm::vec3 rotation_axis,
        long long orbital_period_seconds,
        glm::vec3 orbit_axis,
        TimedSpaceObject* orbit_center,
        float radius) :
        initial_position_(initial_position),
        rotation_period_seconds_(rotation_period_seconds),
        rotation_axis_(rotation_axis),
        orbital_period_seconds_(orbital_period_seconds),
        orbit_axis_(orbit_axis),
        orbit_center_(orbit_center),
        radius_(radius) {}

    glm::mat4 Moon::getSpecificModel(long long current_seconds) {
        glm::mat4 my_model(1.0);

        // translate the moon to its actual position in the world space (considering the position of the orbit center): 4th transformation to be applied
        glm::vec3 current_orbit_center_pos = orbit_center_->getCurrentPosition(current_seconds);
        my_model = glm::translate(my_model, current_orbit_center_pos);

        // rotate planet (on the orbit): 3rd transformation to be applied
        float orbit_angle = computeOrbitAngle(current_seconds);
        my_model = glm::rotate(my_model, orbit_angle, orbit_axis_);

        // translate the moon to its position with respect to the orbit center: 2nd transformation to be applied
        my_model = glm::translate(my_model, initial_position_);

        // rotate moon (rotation): 1st transformation to be applied
        float rotation_angle = computeRotationAngle(current_seconds); // in radians
        my_model = glm::rotate(my_model, rotation_angle, rotation_axis_);

        return my_model;
    }

    glm::vec3 Moon::getCurrentPosition(long long current_seconds) {
        glm::mat4 my_model(1.0);

        // translate the moon to its actual position in the world space (considering the position of the orbit center): 4th transformation to be applied
        glm::vec3 current_orbit_center_pos = orbit_center_->getCurrentPosition(current_seconds);
        my_model = glm::translate(my_model, current_orbit_center_pos);

        // rotate planet (on the orbit): 3rd transformation to be applied
        float orbit_angle = computeOrbitAngle(current_seconds);
        my_model = glm::rotate(my_model, orbit_angle, orbit_axis_);

        // translate the moon to its position with respect to the orbit center: 2nd transformation to be applied
        my_model = glm::translate(my_model, initial_position_);

        return glm::vec3(my_model * glm::vec4(initial_position_, 1.0));
    }

    float Moon::computeRotationAngle(long long current_seconds) {
        current_seconds = current_seconds % rotation_period_seconds_;
        float angle = ((float)current_seconds / (float)rotation_period_seconds_) * 2 * glm::pi<float>();;
        return angle;
    }

    float Moon::computeOrbitAngle(long long current_seconds) {
        current_seconds = current_seconds % orbital_period_seconds_;
        float angle = ((float)current_seconds / (float)orbital_period_seconds_) * 2 * glm::pi<float>();;
        return angle;
    }

    float Moon::getRadius() {
        return radius_;
    }
}