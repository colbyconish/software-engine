#include "pch.h"
#include <SWE/Objects/object.h>


namespace swe
{
    glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
        : position(pos), rotation(rot), scale(s)
    {
    }

    Object::~Object() {}

    void Object::update() {}

    object_ptr Object::createObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
    {
        return object_ptr(new Object(pos, rot, s));
    }

    int Object::addComponent(std::shared_ptr<Component> comp)
    {
        /*if (!(getComponent<Component>() == nullptr))
        {
            std::cout << "Object already has this type of component." << std::endl;
            return 1;
        }*/

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

    int Object::lua_createObject(lua_State *ls)
    {
        lua_newtable(ls);
        lua_pushstring(ls, "x");
        lua_pushnumber(ls, 0);
        lua_settable(ls, -3);

        luaL_getmetatable(ls, "ObjectMeta");
        lua_setmetatable(ls, -2);

        return 1;
    }
} // END namespace swe