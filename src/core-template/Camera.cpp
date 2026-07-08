#include "Camera.hpp"
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos) :
    cameraPosition(pos) {

    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

    cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

    cameraDirection = glm::normalize(
        cameraPosition + cameraFront
    );

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRightVector = glm::normalize(
        glm::cross(up, cameraDirection)
    );

    // cameraUpVector = glm::cross(
    //     cameraDirection,
    //     cameraRightVector
    // );
    cameraUpVector = up;

    _calcViewMatrix();
}

Camera::Camera(glm::vec3 pos, glm::vec3 target) :
    cameraPosition(pos), cameraTarget(target) {

    cameraDirection = glm::normalize(
        cameraPosition + cameraFront
    );

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRightVector = glm::normalize(
        glm::cross(up, cameraDirection)
    );

    cameraUpVector = glm::cross(
        cameraDirection,
        cameraRightVector
    );

    _calcViewMatrix();
}

void Camera::_calcViewMatrix() {
    view = glm::lookAt(
        cameraPosition,
        cameraPosition + cameraFront,
        cameraUpVector
    );
}

void Camera::SetPosition(glm::vec3 pos) {
    cameraPosition = pos;
    _calcViewMatrix();
}

void Camera::SetTarget(glm::vec3 target) {
    cameraTarget = target;
    _calcViewMatrix();
}

void Camera::SetCameraFront(glm::vec3 direction) {
    cameraFront = glm::normalize(direction);
    _calcViewMatrix();
}

void Camera::MoveUp() {
    cameraPosition += glm::normalize(cameraUpVector) * cameraSpeed;
    _calcViewMatrix();
}

void Camera::MoveDown() {
    cameraPosition -= glm::normalize(cameraUpVector) * cameraSpeed;
    _calcViewMatrix();
}

void Camera::MoveRight() {
    cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUpVector)) * cameraSpeed;
    _calcViewMatrix();
}

void Camera::MoveLeft() {
    cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUpVector)) * cameraSpeed;
    _calcViewMatrix();
}

void Camera::MoveFront() {
    cameraPosition += cameraSpeed * cameraFront;
    _calcViewMatrix();
}

void Camera::MoveBack() {
    cameraPosition -= cameraSpeed * cameraFront;
    _calcViewMatrix();
}
