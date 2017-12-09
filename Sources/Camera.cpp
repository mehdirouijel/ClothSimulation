/* ==========================================================================================
 * File Name     : Camera.cpp
 *
 * Creation Date : 07/12/2017 - 06:42
 * Last Modified : 09/12/2017 - 15:37
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Camera.h"


// PUBLIC METHODS
// --------------

Camera::Camera(glm::vec3 position,
               glm::vec3 up,
               float fov,
               float yaw,
               float pitch,
               float speed,
               float sensitivity)
{
    Position = position;
    Speed = speed;
    Sensitivity = sensitivity;
    FOV = fov;
    worldUp_ = up;
    pitch_ = pitch;
    yaw_ = yaw;

    UpdateCamera();
}

glm::mat4
Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + front_, up_);
}

void
Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = Speed * deltaTime;

    if (direction == FORWARD)
    {
        Position += front_ * velocity;
    }
    if (direction == BACKWARD)
    {
        Position -= front_ * velocity;
    }
    if (direction == LEFT)
    {
        Position -= right_ * velocity;
    }
    if (direction == RIGHT)
    {
        Position += right_ * velocity;
    }
    if (direction == UP)
    {
        Position += up_ * velocity;
    }
    if (direction == DOWN)
    {
        Position -= up_ * velocity;
    }
}

void
Camera::ProcessMouse(float xOffset, float yOffset)
{
    yaw_ += xOffset * Sensitivity;
    pitch_ -= yOffset * Sensitivity;

    if (pitch_ > 89.0f)
    {
        pitch_ = 89.0f;
    }
    else if (pitch_ < -89.0f)
    {
        pitch_ = -89.0f;
    }

    UpdateCamera();
}

glm::vec3
Camera::GetFront()
{
    return front_;
}


// PRIVATE METHODS
// ---------------

void
Camera::UpdateCamera()
{
    front_ = glm::normalize(glm::vec3(
        glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_)),
        glm::sin(glm::radians(pitch_)),
        glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_))
    ));

    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
