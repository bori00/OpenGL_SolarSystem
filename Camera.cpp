#include "Camera.hpp"

namespace gps {

    const float Camera::DFEAULT_MOVE_SPEED = 0.01;
    const float Camera::DEFAULT_ROTATION_RADIANS = 0.0314;

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->worldUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        updateCameraRightUpDirection();
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }


    glm::vec3 Camera::getCameraPosition() {
        glm::vec3 res = cameraPosition;
        return res;
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
        glm::mat4 pitch_rotator = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), cameraRightDirection);
        cameraFrontDirection = pitch_rotator * glm::vec4(cameraFrontDirection, 1.0);

        // yaw
        glm::mat4 yaw_rotator = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), cameraUpDirection);
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