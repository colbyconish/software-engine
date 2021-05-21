#include "pch.h"
#include "SWE/SWE.h"

namespace swe
{
    Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
        : Position(pos), WorldUp(up), Yaw(yaw), Pitch(pitch)
    {
        update();
    }

    Camera::Camera(float posX, float posY, float posZ,
                   float upX, float upY, float upZ,
                   float yaw, float pitch)
        : Yaw(yaw), Pitch(pitch)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        update();
    }

    void Camera::update()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 Camera::GetViewMatrix2D() const
    {
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 right = glm::normalize(glm::cross(front, WorldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        return glm::lookAt(position, position + front, up);
    }

    void Camera::addPitch(float pitch)
    {
        Pitch += pitch;
        update();
    }

    void Camera::addYaw(float yaw)
    {
        Yaw += yaw;
        update();
    }
} // END namespace swe
