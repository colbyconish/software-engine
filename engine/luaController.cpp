#include "pch.h"
#include <SWE/Engine/LuaController.h>
#include <SWE/Objects/object.h>

namespace swe
{
	void LuaController::init()
	{
		luaL_newmetatable(ID, "ObjectMeta");

		lua_pushcfunction(ID, Object::lua_createObject);
		lua_setglobal(ID, "createObject");
	}

	LuaController::LuaController()
	{
		ID = luaL_newstate();
		luaL_openlibs(ID);
	}

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
}