#include "pch.h"
#include <SWE/Engine/LuaController.h>
#include <SWE/Objects/object.h>

namespace swe
{
	void LuaController::init()
	{
		//Create interface for Scene class
		lua_newtable(ID);
		lua_setglobal(ID, "Scene");

		//Create interface for Object class
		luaL_newmetatable(ID, lua_object_name);
		lua_pushstring(ID, "__gc");
		lua_pushcfunction(ID, Object::lua_destroyObject);
		lua_settable(ID, -3);

		lua_newtable(ID);
		lua_pushstring(ID, "createObject");
		lua_pushcfunction(ID, Object::lua_createObject);
		lua_settable(ID, -3);

		lua_setglobal(ID, "Object");
	}

	LuaController::LuaController() :ID(luaL_newstate()) { luaL_openlibs(ID); }

	LuaController::~LuaController()
	{
		if(ID != nullptr) lua_close(ID);
	}

	void LuaController::close()
	{
		lua_close(ID);
		ID = nullptr;
	}

	int32_t LuaController::runFile(std::string fileLocation)
	{
		int32_t err = luaL_dofile(ID, fileLocation.c_str());
		if (err != LUA_OK)
		{
			std::string errs = popString(-1);
			std::cout << errs << std::endl;
		}
		return err;
	}

	int32_t LuaController::runString(std::string code)
	{
		return luaL_dostring(ID, code.c_str());
	}

	std::string LuaController::popString(int32_t index)
	{
		return lua_tostring(ID, index);
	}

	void LuaController::createTable()
	{
		lua_newtable(ID);
	}

	void LuaController::putOnTable(int tableIndex, std::string key, int value)
	{
		if (tableIndex == 0)
		{
			std::cout << "Invalid table index." << std::endl;
			return;
		}

		if (tableIndex < 1)
			tableIndex -= 2;

		lua_pushstring(ID, key.c_str());
		lua_pushnumber(ID, value);
		lua_settable(ID, tableIndex);
	}
}