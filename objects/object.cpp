#include "pch.h"
#include <SWE/Objects/object.h>
#include <SWE/Engine/luaController.h>

namespace swe
{
    void test(int x, int y) { printf("test%i", x*y); }

    glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    Object::Object(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
        :position(pos), rotation(rot), scale(s) {}

    Object::Object() :position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), scale(glm::vec3(1.0f)) {}

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

    //lua functions
    int Object::lua_createObject(lua_State *ls)
    {
        void* obj_ptr = lua_newuserdata(ls, sizeof(Object));
        new (obj_ptr) Object();

        luaL_getmetatable(ls, lua_object_name);
        lua_setmetatable(ls, -2);
        return 1;
    }

    int Object::lua_destroyObject(lua_State* ls)
    {
        Object *obj_ptr = (Object *)lua_touserdata(ls, -1);
        obj_ptr->~Object();
        return 0;
    }

    int Object::lua_addComponent(lua_State* ls)
    {

        return 0;
    }

    int Object::lua_getComponent(lua_State* ls)
    {
        return 0;
    }

    int Object::lua_indexObject(lua_State* ls)
    {
        Object* obj_ptr = (Object*)lua_touserdata(ls, -2);
        std::string idx = lua_tostring(ls, -1);

        if (idx == "position")
        {
            lua_pushnumber(ls, obj_ptr->position.x);
            return 1;
        }
        else if (idx == "rotation")
        {
            lua_pushnumber(ls, obj_ptr->rotation.x);
            return 1;
        }
        else if (idx == "scale")
        {
            lua_pushnumber(ls, obj_ptr->scale.x);
            return 1;
        }
        else
        {
            lua_getglobal(ls, "Object");
            lua_pushstring(ls, idx.c_str());
            
            if (lua_rawget(ls, -2) == LUA_TFUNCTION)
                return 1;
            else 
            {
                std::cout << "Object has no member " << idx << std::endl;
                return 1;
            }
        }
    }

    int Object::lua_writeObject(lua_State* ls)
    {
        Object* obj_ptr = (Object*)lua_touserdata(ls, -3);
        std::string idx = lua_tostring(ls, -2);
        float value = (float)lua_tonumber(ls, -1);

        if (idx == "position")
            obj_ptr->position.x = value;
        else if (idx == "rotation")
            obj_ptr->rotation.x = value;
        else if (idx == "scale")
            obj_ptr->scale.x = value;
        else
            std::cout << "Object has no writable member " << idx << std::endl;

        return 0;
    }
} // END namespace swe

