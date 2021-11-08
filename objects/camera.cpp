#include "pch.h"
#include "SWE/SWE.h"

namespace swe
{
    Camera::Camera(glm::vec3 pos, glm::vec3 rot, glm::vec3 s, glm::vec3 up)
        : Object(pos, rot + glm::vec3(0, YAW, 0), s), worldUp(up)
    {
        update();
    }

    void Camera::update()
    {
        glm::vec3 Front;
        Front.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
        Front.y = sin(glm::radians(rotation.x));
        Front.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
        front = glm::normalize(Front);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    /*glm::mat4 Camera::getViewMatrix2D() const
    {
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        return glm::lookAt(position, position + front, up);
    }*/
} // END namespace swe
