#include "pch.h"
#include <SWE/Components/transform.h>

namespace swe
{
    glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    Transform::Transform()
        :Component()
    {
        position = new glm::vec3(0.0f);
        rotation = new glm::vec3(0.0f);
        scale = new glm::vec3(1.0f);
    }

    Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
        :Component()
    {
        position = new glm::vec3(pos);
        rotation = new glm::vec3(rot);
        scale = new glm::vec3(s);
    }

    Transform::~Transform()
    {
        delete position;
        delete rotation;
        delete scale;
    }

    compType Transform::getType() const
    {
        return compTypeFromTemplate<Transform>{}.type;
    }

    glm::mat4 Transform::getModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, *scale);
        model = glm::translate(model, *position);
        model = glm::rotate(model, glm::radians(rotation->x), xAxis);
        model = glm::rotate(model, glm::radians(rotation->y), yAxis);
        model = glm::rotate(model, glm::radians(rotation->z), zAxis);
        
        return model;
    }
}