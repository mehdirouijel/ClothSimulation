#ifndef _CAMERA_H_
#define _CAMERA_H_

/* ==========================================================================================
 * File Name     : Camera.h
 *
 * Creation Date : 07/12/2017 - 06:31
 * Last Modified : 09/12/2017 - 15:37
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

enum CameraMovement : unsigned char
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};


class Camera
{

public:
    glm::vec3 Position;
    float Speed;
    float Sensitivity;
    float FOV;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float fov = 75.0f,
           float yaw = -90.0f,
           float pitch = -30.0f,
           float speed = 2.0f,
           float sensitivity = 0.05f);

    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouse(float xOffset, float yOffset);
    glm::vec3 GetFront();


private:
    glm::vec3 worldUp_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    float pitch_;
    float yaw_;
    bool firstMouseInput_;

    void UpdateCamera();

};


#endif // _CAMERA_H_

