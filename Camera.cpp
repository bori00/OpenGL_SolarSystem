#include "Camera.hpp"

namespace gps {

    const float Camera::DFEAULT_MOVE_SPEED = 0.01;
    const float Camera::DEFAULT_ROTATION_RADIANS = 0.0314;

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->worldUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        updateCameraRightUpDirection();
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition - cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        // TODO: modify cameraTarget
        switch (direction) {
            case MOVE_LEFT: cameraPosition -= cameraRightDirection * speed;
                return;
            case MOVE_RIGHT: cameraPosition += cameraRightDirection * speed;
                return;
            case MOVE_FORWARD: cameraPosition += cameraFrontDirection * speed;
                return;
            case MOVE_BACKWARD: cameraPosition -= cameraFrontDirection * speed;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        glm::vec3 prevCameraFrontDirection = cameraFrontDirection;

        // pitch
        glm::mat4 pitch_rotator = glm::rotate(glm::mat4(1.0f), glm::radians(-pitch), glm::vec3(1, 0, 0));
        cameraFrontDirection = pitch_rotator * glm::vec4(cameraFrontDirection, 1.0);

        // yaw
        glm::mat4 yaw_rotator = glm::rotate(glm::mat4(1.0f), glm::radians(-yaw), glm::vec3(0, 1, 0));
        cameraFrontDirection = yaw_rotator * glm::vec4(cameraFrontDirection, 1.0);

        cameraFrontDirection = glm::normalize(cameraFrontDirection);
        if (cameraFrontDirection == worldUpDirection) {
            cameraFrontDirection = prevCameraFrontDirection;
        }
        else {
            updateCameraRightUpDirection();
        }
    }

    void Camera::updateCameraRightUpDirection() {
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, worldUpDirection));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}