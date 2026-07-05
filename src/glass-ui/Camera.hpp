#pragma once
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

class Camera {
private:
    float cameraSpeed = 0.02f;

    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraDirection;
    glm::vec3 cameraRightVector;
    glm::vec3 cameraUpVector;

    glm::mat4 view = glm::mat4(1.0f);

    void _calcViewMatrix();
public:
    Camera() = delete;
    Camera(glm::vec3 pos);
    Camera(glm::vec3 pos, glm::vec3 target);

    ~Camera() = default;

    glm::mat4 GetViewMatrix() { return view; };

    void SetCameraSpeed(float speed) { cameraSpeed = speed; };

    void SetPosition(glm::vec3 pos);
    void SetTarget(glm::vec3 target);

    void MoveUp();
    void MoveDown();

    void MoveRight();
    void MoveLeft();
};
