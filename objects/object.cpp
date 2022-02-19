#include "pch.h"
#include <SWE/Objects/object.h>
#include <SWE/Components/transform.h>
#include <SWE/Components/model.h>
#include <SWE/Components/script.h>

namespace swe
{
    double test(double x, double y) { return x * y; }

    void Object::print()
    {
        std::shared_ptr<Transform> transform = getComponent<Transform>();
        printf("Object : %d\n", transform.use_count());
        printf("\t%f, %f, %f\n", transform->position->x, transform->position->y, transform->position->z);
        printf("\t%f, %f, %f\n", transform->rotation->x, transform->rotation->y, transform->rotation->z);
        printf("\t%f, %f, %f\n", transform->scale->x, transform->scale->y, transform->scale->z);
    }

    Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
    {
        addComponent(std::shared_ptr<Transform>(new Transform(pos, rot, s)));
    }

    Object::Object() 
    {
        addComponent(std::shared_ptr<Transform>(new Transform()));
    }

    Object::~Object() { printf("%s\n", "Object died"); }

    void Object::update() {}

    object_ptr Object::createObject()
    {
        return object_ptr(new Object(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
    }

    std::shared_ptr<Component> Object::luaGetComponent(const char* t) const
    {
        compType type = Component::stringToCompType(t);

        switch (type)
        {
        case compType::transform:
            return getComponent<Transform>();
            break;

        case compType::model:
            return getComponent<Model>();
            break;

        case compType::script:
            return getComponent<Script>();
            break;

        default:
            std::cout << "Invalid component type in luaGetComponent." << std::endl;
        }

        return nullptr;
    }

    bool Object::addComponent(std::shared_ptr<Component> comp)
    {
        compType type = comp->getType();
        if (type == compType::base)
        {
            std::cout << "Passed in unusable component." << std::endl;
            return 0;
        }   

        auto it = components.find(type);
        if (it == components.end())
        {
            std::pair<compType, std::shared_ptr<Component>> pair(type, comp);
            components.insert(pair);
            return 1;
        }
        else
        {
            it->second = comp;
            return 1;
        }

        return 0;
    }

} // END namespace swe

