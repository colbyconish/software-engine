#include "pch.h"
#include <SWE/Objects/object.h>
#include <SWE/Components/transform.h>
#include <SWE/Components/model.h>
#include <SWE/Engine/error.h>

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
        : visible(true)
    {
        addComponent(std::shared_ptr<Transform>(new Transform(pos, rot, s)));
    }

    Object::Object() 
        : visible(true)
    {
        addComponent(std::shared_ptr<Transform>(new Transform()));
    }

    Object::~Object() 
    {
        for (auto c : components)
            c.second->parent = nullptr;

        for (auto s : scripts)
            s.second->parent = nullptr;
    }

    void Object::update() 
    {
        for (auto c : components)
            c.second->update();

        for (auto s : scripts)
            s.second->update();
    }

    void Object::close()
    {
        for (auto c : components)
            c.second->close();

        for (auto s : scripts)
            s.second->close();
    }

    object_ptr Object::createObject()
    {
        return object_ptr(new Object());
    }

    object_ptr Object::createPrimitive(primitiveType p)
    {
        object_ptr obj = object_ptr(new Object());
        model_ptr model = Model::createModel();
        obj->addComponent(model);

        std::vector<Vertex> verts;
        std::vector<uint32_t> indis;

        switch (p)
        {
        case primitiveType::Cube:
            verts = {
                Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)}, Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)},
                Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f)}, Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f)},
                Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)}, Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
                Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)}, Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
                Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)}, //[2] copy
                Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 0.0f)}, //[7] copy
                Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},//[6] copy
                Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 1.0f)} //[0] copy
            };

            indis = {
                2, 0, 1,   2, 1, 3,  //Front
                4, 9, 5,   4, 6, 9,  //Back
                11, 4, 5,  11, 5, 1, //Top
                10, 8, 3,  10, 3, 7, //Bottom
                0, 6, 4,   0, 8, 6,  //Left
                5, 3, 1,   5, 7, 3   //Right
            };
            break;
        default:
            Error err = Error("Primitive not supported.", errorLevel::Error, __SOURCELOCATION__);
        }

        mesh_ptr mesh = Mesh::createMesh(verts, indis);
        model->addMesh(mesh);

        return obj;
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
        case compType::light:
            return getComponent<Light>();
            break;
        default:
            Error err = Error("Invalid component type.", errorLevel::Error, __SOURCELOCATION__);
        }

        return nullptr;
    }

    component_ptr Object::getScriptByName(const char* t) const
    {
        script_ptr script = nullptr;
        std::string name = std::string(t);
        for (auto component : scripts)
        {
            script_ptr temp = std::dynamic_pointer_cast<Script>(component.second);
            if (temp->name == name)
                script = temp;
        }

        return script;
    }

    bool Object::addComponent(std::shared_ptr<Component> comp)
    {
        compType type = comp->getType();
        if (type == compType::base || type == compType::null)
        {
            Error err = Error("Passed in unusable component.", errorLevel::Error, __SOURCELOCATION__);
            return 0;
        }

        uint32_t index;
        std::map<uint32_t, component_ptr>* map;
        if (type == compType::script)
        {
            Script* s = (Script*)comp.get();
            map = &scripts;
            index = s->ID;
        }
        else
        {
            map = &components;
            index = (uint32_t)type;
        }


        auto it = map->find(index);
        if (it == map->end())
        {
            std::pair<uint32_t, std::shared_ptr<Component>> pair(index, comp);
            map->insert(pair);

            comp->parent = this;
            comp->init();
            return 1;
        }
        else
        {
            it->second->parent = nullptr;
            it->second = comp;

            comp->parent = this;
            comp->init();
            return 1;
        }

        comp->parent = this;
        return 0;
    }

    Window* Component::getWindow()
    {
        if (parent == nullptr)
            return nullptr;
        return parent->getWindow();
    }

} // END namespace swe

