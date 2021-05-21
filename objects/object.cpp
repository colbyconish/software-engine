#include "pch.h"
#include "SWE/SWE.h"

namespace swe
{
    Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
        : position(pos), rotation(rot), scale(s), aContainer(false), ID(num_objects++)
    {
    }

    Object::~Object() {}

    int Object::addComponent(std::shared_ptr<Component> comp)
    {
        if (!(getComponent<Component>(comp->type) == nullptr))
        {
            std::cout << "Object already has this type of component." << std::endl;
            return 1;
        }

        components.push_back(comp);
        return 0;
    }

    glm::mat4 Object::getModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians(rotation.x), xAxis);
        model = glm::rotate(model, glm::radians(rotation.y), yAxis);
        model = glm::rotate(model, glm::radians(rotation.z), zAxis);
        model = glm::translate(model, position);

        return model;
    }

    bool Object::isUI() { return false; }

    bool Object::isContainer() { return aContainer; }
} // END namespace swe