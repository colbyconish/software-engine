#include "pch.h"
#include <SWE/Engine/window.h>
#include <SWE/Components/script.h>
#include <SWE/Engine/luaController.h>
#include <SWE/Engine/error.h>

namespace swe
{
	int32_t Script::num_scripts = 0;
	const std::string Script::magicFunctionNames[] =
	{
		"onInit",
		"onUpdate",
		"onScrollWheel",
		"onKeyPressed",
		"onKeyReleased",
		"onMousePressed",
		"onMouseReleased"
	};

	Script::Script(const char* fileLocation)
		: Component(), magicFunctions(std::map<std::string, int32_t>()) , name(fileLocation), ID(num_scripts++){ }

	Script::~Script() 
	{
		if (LuaController::checkInit())
		{
			for (auto func : magicFunctions)
				if (func.second != -1)
					LuaController::unrefRegistryIndex(func.second);
		}

		if (getWindow() != nullptr)
			getWindow()->onInput.UNHOOK(&Script::onInput);
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
		for (std::string name : magicFunctionNames)
			magicFunctions[name] = LuaController::getLuaFunction(name);
		if (magicFunctions["onInit"] != -1) LuaController::callRegistryFunction(magicFunctions["onInit"]);

		LuaController::pushNil();
		LuaController::setGlobal("this");
		for (std::string name : magicFunctionNames)
			LuaController::pushNil(), LuaController::setGlobal(name);

		if (requiresInput())
			getWindow()->onInput.HOOK(&Script::onInput);
	}

	compType Script::getType() const
	{
		return compTypeFromTemplate<Script>{}.type;
	}

	void Script::update() 
	{
		if (magicFunctions["onUpdate"] == -1) return;
		LuaController::callRegistryFunction(magicFunctions["onUpdate"]);
	}

	void Script::onInput(void* ptr, Input I)
	{
		Script* script = (Script*)ptr;
		switch (I.type)
		{
		case inputType::key:
			if (I.data.d_data.pressed && script->magicFunctions["onKeyPressed"] != -1)
				LuaController::callRegistryFunction(script->magicFunctions["onKeyPressed"], 1, I.data.d_data.button);
			else if (!I.data.d_data.pressed && script->magicFunctions["onKeyReleased"] != -1)
				LuaController::callRegistryFunction(script->magicFunctions["onKeyReleased"], 1, I.data.d_data.button);
			break;
		case inputType::mouse:
			if (I.data.d_data.pressed && script->magicFunctions["onMousePressed"] != -1)
				LuaController::callRegistryFunction(script->magicFunctions["onMousePressed"], 1, I.data.d_data.button);
			else if (!I.data.d_data.pressed && script->magicFunctions["onMouseReleased"] != -1)
				LuaController::callRegistryFunction(script->magicFunctions["onMouseReleased"], 1, I.data.d_data.button);
			break;
		case inputType::scroll:
			if(script->magicFunctions["onScrollWheel"] != -1)
				LuaController::callRegistryFunction(script->magicFunctions["onScrollWheel"], 2, I.data.a_data.xoffset, I.data.a_data.yoffset);
			break;
		}
	}

	bool Script::requiresInput()
	{
		bool needsKey = magicFunctions["onKeyPressed"] != -1 || magicFunctions["onKeyReleased"] != -1;
		bool needsMouse = magicFunctions["onMousePressed"] != -1 || magicFunctions["onMouseReleased"] != -1;
		
		return needsMouse || needsKey;
	}
}