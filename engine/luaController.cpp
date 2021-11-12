#include "pch.h"
#include <SWE/Engine/LuaController.h>
#include <rttr/registration>

namespace swe
{
	lua_State *LuaController::ID = nullptr;

	void LuaController::init()
	{
		ID = luaL_newstate();
		luaL_openlibs(ID);

		//puts globals table on stack
		lua_newtable(ID);
		lua_pushvalue(ID, -1);
		lua_setglobal(ID, "Game");

		lua_pushvalue(ID, -1);
		//push all global functions
		for (auto& method : rttr::type::get_global_methods())
		{
			lua_pushstring(ID, method.get_name().c_str());
			lua_pushvalue(ID, -1);
			lua_pushcclosure(ID, callLuaGlobals, 1);
			lua_settable(ID, -3);
		}
	}

	LuaController::~LuaController()
	{
		if(ID != nullptr) lua_close(ID);
	}

	int LuaController::callLuaGlobals(lua_State* ls)
	{
		rttr::method method = rttr::type::get_global_method(lua_tostring(ls, lua_upvalueindex(1)));
		std::vector<rttr::parameter_info> params = method.get_parameter_infos();

		std::string name = method.get_name();
		int numArgsPassed = lua_gettop(ls);
		if (numArgsPassed != (int)params.size())
			std::cout << "Error calling function '" << name << "'. Wrong number of arguments provided." << std::endl;

		union Value
		{
			int intValue;
		};

		std::vector<Value> values(params.size());
		std::vector<rttr::argument> args(params.size());
		for (int i = 0; i < numArgsPassed; i++)
		{
			rttr::type paramType = params[i].get_type();
			int luaType = lua_type(ls, i+1);

			switch (luaType)
			{
			case LUA_TNIL:

				break;
			case LUA_TBOOLEAN:

				break;
			case LUA_TLIGHTUSERDATA:

				break;
			case LUA_TNUMBER:
				if (paramType == rttr::type::get<int>())
				{
					values[i].intValue = (int)lua_tonumber(ls, i + 1);
					args[i] = values[i].intValue;
				}
				break;
			case LUA_TSTRING:

				break;
			case LUA_TTABLE:

				break;
			case LUA_TFUNCTION:

				break;
			case LUA_TUSERDATA:

				break;
			case LUA_TTHREAD:

				break;
			default:
				std::cout << "Unknown lua type " << luaType << "." << std::endl;
				break;
			}
		}

		rttr::variant variant = method.invoke_variadic({}, args);

		if (!variant.is_valid())
			std::cout << "Failed to call native function '" << name << "'." << std::endl;
		return 0;
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

	/*void LuaController::makeLuaObjectTables()
	{
		//making public object table
		lua_newtable(ID);
		int obj_table = lua_gettop(ID);

		lua_pushstring(ID, "new");
		lua_pushcfunction(ID, Object::lua_createObject);
		lua_settable(ID, -3);

		lua_pushstring(ID, "getComponent");
		lua_pushcfunction(ID, Object::lua_getComponent);
		lua_settable(ID, -3);

		lua_pushstring(ID, "addComponent");
		lua_pushcfunction(ID, Object::lua_addComponent);
		lua_settable(ID, -3);

		lua_settop(ID, obj_table);
		lua_setglobal(ID, "Object");
		
		//making meta object table
		luaL_newmetatable(ID, lua_object_name);

		lua_pushstring(ID, "__gc");
		lua_pushcfunction(ID, Object::lua_destroyObject);
		lua_settable(ID, -3);

		lua_pushstring(ID, "__index");
		lua_pushcfunction(ID, Object::lua_indexObject);
		lua_settable(ID, -3);

		lua_pushstring(ID, "__newindex");
		lua_pushcfunction(ID, Object::lua_writeObject);
		lua_settable(ID, -3);
	}*/
}

