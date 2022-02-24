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

    Object::~Object() { }

    void Object::update() 
    {
        transform_ptr t = getComponent<Transform>();
        t->rotation->x += 1.0f;

        t->position->x = (float) sin(glfwGetTime());
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
                0, 1, 2,   1, 3, 2,  //Front
                5, 4, 9,   4, 6, 9,  //Back
                4, 5, 11,  5, 1, 11, //Top
                8, 3, 10,  3, 7, 10, //Bottom
                4, 0, 6,   0, 8, 6,  //Left
                1, 5, 3,   5, 7, 3   //Right
            };
            break;
        default:
            std::cout << "Primitive not supported." << std::endl;
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

