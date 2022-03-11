#include "pch.h"
#include <SWE/Components/script.h>
#include <SWE/Engine/luaController.h>

namespace swe
{
	uint32_t Script::num_scripts = 0;

	Script::Script(const char* fileLocation)
		: Component(), onUpdate(-1), name(fileLocation), ID(num_scripts++) {	}

	Script::~Script() 
	{
		if (onUpdate != -1)
			LuaController::unrefRegistryIndex(onUpdate);
	}

	script_ptr Script::createScript(const char* fileLocation)
	{
		return script_ptr(new Script(fileLocation));
	}

	void Script::init()
	{
		script_ptr temp = std::dynamic_pointer_cast<Script>(shared_from_this());

		LuaController::pushShared(temp);
		LuaController::setGlobal("this");
		LuaController::runFile(name);
		LuaController::pushNil();
		LuaController::setGlobal("this");
	}

	compType Script::getType() const
	{
		return compTypeFromTemplate<Script>{}.type;
	}

	void Script::update() 
	{
		if (onUpdate == -1)
			return;
		LuaController::pushRegistryIndex(onUpdate);
		LuaController::callFunction();
	}
}