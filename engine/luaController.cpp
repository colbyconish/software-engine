#include "pch.h"
#include <SWE/Engine/LuaController.h>
#include <SWE/Objects/object.h>
#include <SWE/Components/transform.h>
#include <SWE/Engine/vector.h>
#include <SWE/Engine/scene.h>
#include <SWE/Components/script.h>
#include <SWE/Engine/error.h>

namespace swe
{
	lua_State *LuaController::ID = nullptr;
	std::recursive_mutex LuaController::callLock = std::recursive_mutex();
	//const char* LuaController::magicFunctionFile = "./magicFunctions.lua";

	LuaController::~LuaController()
	{
		if (ID != nullptr) lua_close(ID);
	}

	void LuaController::init()
	{
		ID = luaL_newstate();
		luaL_openlibs(ID);

		//puts globals table on stack
		lua_newtable(ID);
		int gameTable = lua_gettop(ID);
		lua_pushvalue(ID, gameTable);
		lua_setglobal(ID, "Game");

		//push all global functions
		for (auto& method : rttr::type::get_global_methods())
		{
			lua_pushstring(ID, method.get_name().data());
			lua_pushvalue(ID, -1);
			lua_pushcclosure(ID, callLuaGlobals, 1);
			lua_settable(ID, gameTable);
		}

		//push all classes
		for (auto& type : rttr::type::get_types())
		{
			if (type.is_class() && !type.is_wrapper() && !type.is_template_instantiation())
			{
				printf("%s\n", type.get_name().data());
				std::string typeName = type.get_name().to_string();

				lua_newtable(ID);
				lua_pushstring(ID, type.get_name().data());
				lua_pushvalue(ID, -2);
				lua_settable(ID, gameTable);
				
				lua_pushstring(ID, type.get_name().data());
				lua_pushcclosure(ID, createLightUserData, 1);
				lua_setfield(ID, -2, "new");

				luaL_newmetatable(ID, metaName(type).c_str());
				lua_pushstring(ID, "__gc");
				lua_pushcclosure(ID, garbageCollection, 0);
				lua_settable(ID, -3);

				lua_pushstring(ID, "__index");
				lua_pushstring(ID, type.get_name().data());
				lua_pushcclosure(ID, readObject, 1);
				lua_settable(ID, -3);

				lua_pushstring(ID, "__newindex");
				lua_pushstring(ID, type.get_name().data());
				lua_pushcclosure(ID, writeObject, 1);
				lua_settable(ID, -3);

				setMetaTableInfo(ID, typeName);
			}
		}
	}

	inline bool LuaController::checkInit()
	{
		bool inited = ID != nullptr;

		if (!inited)
			Error err = Error("LuaController is not initialized.", errorLevel::Warning, __SOURCELOCATION__);

		return inited;
	}

	std::string LuaController::metaName(rttr::type type)
	{
		if (type.is_wrapper())
			return type.get_wrapped_type().get_raw_type().get_name().to_string() + "Meta";
		else if (type.is_pointer())
			return type.get_raw_type().get_name().to_string() + "Meta";
		
		return type.get_name().to_string() + "Meta";
	}
		

	void LuaController::setMetaTableInfo(lua_State* ls, std::string name)
	{
		lua_pushstring(ls, "__shared_ptr");

		if (name == "Vector3")
			lua_pushboolean(ls, false);
		else
			lua_pushboolean(ls, true);

		lua_settable(ID, -3);
	}

	int LuaController::writeObject(lua_State* ls)
	{
		rttr::type type = rttr::type::get_by_name(lua_tostring(ls, lua_upvalueindex(1)));
		void* pointer = lua_touserdata(ls, 1);
		const char* index = lua_tostring(ls, 2);

		rttr::property prop = type.get_property(index);
		if (prop.is_valid())
		{
			rttr::type propType = prop.get_type(); 
			rttr::instance instance = getInstance(type, pointer);

			Value value[1];
			rttr::argument arg[1];
			getArgument(propType, &value[0], &arg[0], lua_type(ls, 3), ls, 0, 3);

			rttr::variant result = prop.set_value(instance, arg[0]);
			return 0;
		}

		lua_getuservalue(ls, 1);
		lua_pushvalue(ls, 2);
		lua_pushvalue(ls, 3);
		lua_settable(ls, -3);
		return 0;
	}

	int LuaController::readObject(lua_State* ls)
	{
		rttr::type type = rttr::type::get_by_name(lua_tostring(ls, lua_upvalueindex(1)));
		object_ptr* pointer = ((object_ptr*)lua_touserdata(ls, 1));
		const char* index = lua_tostring(ls, 2);

		std::string t = type.get_name().data();

		//check if method exists with index name
		rttr::method method = type.get_method(index);
		if (!method.is_valid())
		{
			//check if property exists with index name
			rttr::property prop = type.get_property(index);
			if (!prop.is_valid())
			{
				//check if uservalue exists with index name
				lua_getuservalue(ls, 1);
				lua_pushvalue(ls, 2);
				lua_gettable(ls, -2);
				return 1;
			}

			rttr::instance instance = getInstance(type, pointer);
			rttr::variant result = prop.get_value(instance);
			if (!result.is_valid())
			{
				std::cout << "Invalid result for object index " << type.get_name().data() << std::endl;
				return 0;
			}

			return getReturnValue(result, ls);
		}
		
		rttr::instance instance = getInstance(type, pointer);

		void* method_ptr = lua_newuserdata(ls, sizeof(rttr::method));
		void* instance_ptr = lua_newuserdata(ls, sizeof(rttr::instance));

		new (method_ptr) rttr::method(method);
		new (instance_ptr) rttr::instance(instance);
		lua_pushcclosure(ls, callIndexFunctions, 2);

		return 1;
	}

	int LuaController::addExtraUserValues(lua_State* ls, rttr::type type, int idx)
	{
		if (rttr::type::get<script_ptr>() == type)
		{
			std::cout << "hell yeah" << std::endl;
		}

		return 0;
	}

	int LuaController::garbageCollection(lua_State *ls)
	{
		void_ptr* ptr = (void_ptr*)lua_touserdata(ls, 1);

		lua_getmetatable(ls, 1);
		lua_getfield(ls, -1, "__shared_ptr");

		if (lua_toboolean(ls, -1))
			ptr->~shared_ptr();

		return 0;
	}

	bool LuaController::getReturnValue(rttr::variant& result, lua_State* ls)
	{
		if (result.is_type<int>())
		{
			lua_pushnumber(ls, result.get_value<int>());
			return true;
		}
		else if (result.is_type<double>())
		{
			lua_pushnumber(ls, result.get_value<double>());
			return true;
		}
		else if (result.is_type<float>())
		{
			lua_pushnumber(ls, result.get_value<float>());
			return true;
		}
		if (result.is_type<bool>())
		{
			lua_pushboolean(ls, result.get_value<bool>());
			return true;
		}
		else if (result.is_type<lua_CFunction>())
		{
			lua_pushcfunction(ls, result.get_value<lua_CFunction>());
			return true;
		}
		else if (result.is_type<Object *>())
		{
			if (pushSharedToLua<object_ptr, Object *>(ls, result)) { return true; }
		}
		else
		{
			Error err = Error("Return value may be incorrectly interpreted.", errorLevel::Warning, __SOURCELOCATION__);
			/*
			* for all datatype stored in shared_ptrs or regular pointers
			* glm::vec3 is stored in a regular ptr 
			*/
			if (result.get_type().is_wrapper())
			{
				if (pushSharedToLua<component_ptr>(ls, result)) { return true; }
				else if (pushSharedToLua<object_ptr>(ls, result)) { return true; }
			}
			else if (result.get_type().is_pointer())
			{
				return pushPointerToLua(ls, result);
			}
		}

		Error err = Error(std::string("Unhandled return type ") + result.get_type().get_name().data() + ".", errorLevel::Error, __SOURCELOCATION__);
		return false;
	}

	bool LuaController::pushPointerToLua(lua_State* ls, rttr::variant& result)
	{
		if (result.get_value<void*>() == nullptr)
			return false;

		void* ptr = lua_newuserdata(ls, sizeof(void*));
		new (ptr) void* (result.get_value<void*>());

		luaL_getmetatable(ls, metaName(result.get_type()).c_str());
		lua_setmetatable(ls, -2);

		lua_newtable(ls);
		lua_setuservalue(ls, -2);

		return true;
	}

	rttr::instance LuaController::getInstance(rttr::type type, void* ptr)
	{
		if (type == rttr::type::get_by_name("Vector3"))
		{
			rttr::instance inst = *(glm::vec3**)ptr;

			return inst;
		}
		else if (type == rttr::type::get_by_name("Object"))
		{
			rttr::instance wrapper = (*(object_ptr*)ptr);
			rttr::instance inst = wrapper.get_wrapped_instance();

			return inst;
		}
		else if (type == rttr::type::get_by_name("Transform"))
		{
			rttr::instance wrapper = (*(transform_ptr*)ptr);
			rttr::instance inst = wrapper.get_wrapped_instance();

			return inst;
		}
		else if (type == rttr::type::get_by_name("Light"))
		{
			rttr::instance wrapper = (*(light_ptr*)ptr);
			rttr::instance inst = wrapper.get_wrapped_instance();

			return inst;
		}
		else if (type == rttr::type::get_by_name("Script"))
		{
			rttr::instance wrapper = (*(script_ptr*)ptr);
			rttr::instance inst = wrapper.get_wrapped_instance();

			return inst;
		}
		else
			Error err = Error("Instancing not supported for " + type.get_name().to_string(), errorLevel::Error, __SOURCELOCATION__);

		return rttr::instance();
	}

	int LuaController::createLightUserData(lua_State* ls)
	{
		rttr::type type = rttr::type::get_by_name(lua_tostring(ls, lua_upvalueindex(1)));

		rttr::variant result = type.get_method("new").invoke({});

		if (!result.is_valid()) std::cout << "Failed to create instance of '" << type.get_name() << "'." << std::endl;
		else if (getReturnValue(result, ls)) { return 1; }
		else std::cout << "Unrecognized return type '" << result.get_type().get_name().data() << "' when creating '" << type.get_name().data() << "'." << std::endl;
		
		return 0;
	}

	int LuaController::invokeLuaMethods(lua_State* ls, rttr::method &method, rttr::instance &instance)
	{
		uint8_t obj_offset = instance.is_valid(); //if an instance is passed into function decrement args passed and increment args start index
		const auto& params = method.get_parameter_infos();

		const auto& name = method.get_name();
		int numArgsPassed = lua_gettop(ls);
		if (numArgsPassed - obj_offset != (int)params.size())
			std::cout << "Error calling function '" << name << "'. Wrong number of arguments provided." << std::endl;

		std::vector<Value> values(params.size());
		std::vector<rttr::argument> args(params.size());

		int i = 0;
		for (auto param : params)
		{
			rttr::type paramType = param.get_type(); 
			int luaType = lua_type(ls, i + obj_offset + 1);//+1 to skip over the int representing num args passes; +obj_iffset to skip over object passed

			if (!getArgument(paramType, &values[0], &args[0], luaType, ls, i, obj_offset+1))//+1 to skip over the int representing num args passes; +obj_iffset to skip over object passed
				std::cout << "Unrecognized arg in call to function" << name << std::endl;
			i++;
		}

		rttr::variant result = method.invoke_variadic(instance, args);

		if (!result.is_valid())
			std::cout << "Failed to call native function '" << name << "'." << std::endl;
		else if (!result.is_type<void>())
		{
			if (!getReturnValue(result, ls))
			{
				Error err = Error(std::string("Unrecognized return type '") + result.get_type().get_name().data() + "' on function '" + name + "'.", errorLevel::Error, __SOURCELOCATION__);
				return 0;
			}
			return 1;
		}

		return 0;
	}

	bool LuaController::getArgument(rttr::type& paramType, Value* values, rttr::argument* args, int luaType, lua_State* ls, int i, int lua_offset)
	{
		switch (luaType)
		{
		case LUA_TNIL:
			std::cout << "No support for nil arguments." << std::endl;
			return 0;
			break;
		case LUA_TBOOLEAN:
			if (tryArgument<bool>(paramType, &(values[i]), &(args[i]), (bool)lua_toboolean(ls, i + lua_offset))) { return 1; }
			else
			{
				std::cout << "Function used unsupported boolean type." << std::endl;
				return 0;
			}
			break;
		case LUA_TLIGHTUSERDATA:
			std::cout << "No support for lightuserdata arguments." << std::endl;
			return 0;
			break;
		case LUA_TNUMBER:
			if (tryArgument<double>(paramType, &(values[i]), &(args[i]), (double)lua_tonumber(ls, i + lua_offset))) { return 1; }
			else if (tryArgument<long>(paramType, &(values[i]), &(args[i]), (long)lua_tointeger(ls, i + lua_offset))) { return 1; }
			else if (tryArgument<int>(paramType, &(values[i]), &(args[i]), (int)lua_tointeger(ls, i + lua_offset))) { return 1; }
			else if (tryArgument<short>(paramType, &(values[i]), &(args[i]), (short)lua_tointeger(ls, i + lua_offset))) { return 1; }
			else if (tryArgument<float>(paramType, &(values[i]), &(args[i]), (float)lua_tonumber(ls, i + lua_offset))) { return 1; }
			else
			{
				std::cout << "Function used unsupported number type." << std::endl;
				return 0;
			}
			break;
		case LUA_TSTRING:
			if (tryArgument<const char*>(paramType, &(values[i]), &(args[i]), lua_tostring(ls, i + lua_offset))) { return 1; }
			else std::cout << "Function used unsupported string type." << std::endl;
			return 0;
			break;
		case LUA_TTABLE:
			std::cout << "No support for table arguments." << std::endl;
			return 0;
			break;
		case LUA_TFUNCTION:
			if (tryArgument<int>(paramType, &(values[i]), &(args[i]), luaL_ref(ls, LUA_REGISTRYINDEX))) { return 1; }
			else Error err = Error("Function used unsupported function type.", errorLevel::Error, __SOURCELOCATION__);
			return 0;
			break;
		case LUA_TUSERDATA:
			std::cout << "No support for userdata arguments." << std::endl;
			return 0;
			break;
		case LUA_TTHREAD:
			std::cout << "No support for thread arguments." << std::endl;
			return 0;
			break;
		default:
			std::cout << "Unknown lua type " << luaType << "." << std::endl;
			return 0;
			break;
		}
	}

	int LuaController::callLuaGlobals(lua_State* ls)
	{
		rttr::method method = rttr::type::get_global_method(lua_tostring(ls, lua_upvalueindex(1)));
		
		if (!method.is_valid())
		{
			std::cout << "Global method " << method.get_name() << " does not exist." << std::endl;
			return 0;
		}

		rttr::instance instance = {};
		return invokeLuaMethods(ls, method, instance);
	}

	int LuaController::callIndexFunctions(lua_State *ls)
	{
		rttr::method method = *(rttr::method*)(lua_touserdata(ls, lua_upvalueindex(1)));
		rttr::instance instance = *(rttr::instance*)lua_touserdata(ls, lua_upvalueindex(2));

		return invokeLuaMethods(ls, method, instance);
	}

	int LuaController::callFunction(int numArgs)
	{
		if (!checkInit())
			return -1;

		if (lua_type(ID, -1-numArgs) != LUA_TFUNCTION)
		{
			Error err = Error("Tried running a non-function.", errorLevel::Error, __SOURCELOCATION__);
			return -2;
		}
		else
			if (lua_pcall(ID, numArgs, 0, 0) != 0)
			{
				Error err = Error("Error running function: " + std::string(lua_tostring(ID, -1)), errorLevel::Error, __SOURCELOCATION__);
				return -3;
			}

		return 0;
	}

	void LuaController::setGlobal(std::string name)
	{
		if (checkInit())
			lua_setglobal(ID, name.c_str());
	}

	void LuaController::pushRegistryIndex(int r)
	{
		if (checkInit())
			lua_rawgeti(ID, LUA_REGISTRYINDEX, r);
	}

	void LuaController::unrefRegistryIndex(int r)
	{
		if(checkInit())
			luaL_unref(ID, LUA_REGISTRYINDEX, r);
	}

	void LuaController::pushNil()
	{
		if (checkInit())
			lua_pushnil(ID);
	}

	int LuaController::getLuaFunction(std::string name)
	{
		if (!checkInit())
			return -1;

		int temp = -1;
		lua_getglobal(ID, name.c_str());
		if (lua_isfunction(ID, -1))
			temp = luaL_ref(ID, LUA_REGISTRYINDEX);
		lua_pop(ID, -1);
		return temp;
	}

	void LuaController::callRegistryFunction(int regIndex)
	{
		callLock.lock();
		pushRegistryIndex(regIndex);
		callFunction(0);
		callLock.unlock();
	}

	void LuaController::callGlobalFunction(std::string name)
	{
		callLock.lock();
		lua_getglobal(ID, name.c_str());
		if (lua_type(ID, -1) == LUA_TFUNCTION)
			callFunction(0);
		else
			Error("Tried calling non-function: " + name, errorLevel::Error, __SOURCELOCATION__);
		callLock.unlock();
	}


	void LuaController::close()
	{
		if (checkInit())
			lua_close(ID);
		ID = nullptr;
	}

	int32_t LuaController::runFile(std::string fileLocation)
	{
		if (!checkInit())
			return -1;

		printf("START Output--%s----------\n", fileLocation.c_str());
		int32_t err = luaL_dofile(ID, fileLocation.c_str());
		if (err != LUA_OK)
		{
			std::string errs = popString(-1);
			std::cout << errs << std::endl;
		}
		printf("END Output----%s----------\n", fileLocation.c_str());
		return err;
	}

	int32_t LuaController::runString(std::string code)
	{
		if (!checkInit())
			return -1;
		return luaL_dostring(ID, code.c_str());
	}

	std::string LuaController::popString(int32_t index)
	{
		if (!checkInit())
			return "";
		return lua_tostring(ID, index);
	}

	//rttr Registration
	RTTR_REGISTRATION
	{
	rttr::registration::method("test", &test);

	//Vector class
	rttr::registration::class_<glm::vec3*>("Vector3*");
	rttr::registration::class_<glm::vec3>("Vector3")
		.property("x", &glm::vec3::x)
		.property("y", &glm::vec3::y)
		.property("z", &glm::vec3::z);

	//Component class
	rttr::registration::class_<Component>("Component");
	rttr::registration::class_<transform_ptr>("transform_ptr");
	rttr::registration::class_<Transform>("Transform")
		.property("position", &Transform::position)
		.property("rotation", &Transform::rotation)
		.property("scale", &Transform::scale);
	rttr::registration::class_<light_ptr>("light_ptr");
	rttr::registration::class_<Light>("Light")
		.property("color", &Light::color)
		.property("ambient", &Light::ambient)
		.property("diffuse", &Light::diffuse)
		.property("specular", &Light::specular);
	rttr::registration::class_<script_ptr>("script_ptr");
	rttr::registration::class_<Script>("Script")
		.property("parent", &Script::parent);

	//Object class
	rttr::registration::class_<object_ptr>("object_ptr");
	rttr::registration::class_<Object>("Object")
		.property("visible", &Object::visible)
		.method("new", &Object::createObject)
		.method("print", &Object::print)
		//.method("addComponent", &Object::addComponent)
		.method("getComponent", &Object::luaGetComponent)
		.method("getScriptByName", &Object::getScriptByName);

	//Scene class
	rttr::registration::class_<scene_ptr>("scene_ptr");
	rttr::registration::class_<Scene>("Scene");

	}
}

