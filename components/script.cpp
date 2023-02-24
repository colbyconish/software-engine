#include "pch.h"
#include <SWE/Engine/window.h>
#include <SWE/Components/script.h>
#include <SWE/Engine/luaController.h>
#include <SWE/Engine/error.h>

namespace swe
{
	uint32_t Script::num_scripts = 0;

	Script::Script(const char* fileLocation)
		: Component(), onUpdate(-1), name(fileLocation), ID(num_scripts++) { }

	Script::~Script() 
	{
		if (LuaController::checkInit())
		{
			if (onUpdate != -1)
				LuaController::unrefRegistryIndex(onUpdate);
			if (onKeyPressed != -1)
				LuaController::unrefRegistryIndex(onKeyPressed);
			if (onKeyReleased != -1)
				LuaController::unrefRegistryIndex(onKeyReleased);
			if (onInit != -1)
				LuaController::unrefRegistryIndex(onInit);
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
		getMagicFunctions();
		if (onInit != -1)LuaController::callRegistryFunction(onInit);

		LuaController::pushNil();
		LuaController::setGlobal("this");
		clearMagicFunctions();

		if (requiresInput())
			getWindow()->onInput.HOOK(&Script::onInput);
	}

	compType Script::getType() const
	{
		return compTypeFromTemplate<Script>{}.type;
	}

	void Script::update() 
	{
		if (onUpdate == -1) return;
		LuaController::callRegistryFunction(onUpdate);
	}

	void Script::onInput(void* ptr, Input I)
	{
		Script* script = (Script*)ptr;
		switch (I.type)
		{
		case inputType::key:
			if (I.pressed && script->onKeyPressed != -1)
				LuaController::callRegistryFunction(script->onKeyPressed, I.button, 1);
			else if (!I.pressed && script->onKeyReleased != -1)
				LuaController::callRegistryFunction(script->onKeyReleased, I.button, 1);
			break;
		}
	}

	bool Script::requiresInput()
	{
		return onKeyPressed != -1 || onKeyReleased != -1;
	}

	void Script::getMagicFunctions()
	{
		onUpdate = LuaController::getLuaFunction("onUpdate");
		onKeyPressed = LuaController::getLuaFunction("onKeyPressed");
		onKeyReleased = LuaController::getLuaFunction("onKeyReleased");
		onInit = LuaController::getLuaFunction("onInit");
	}

	void Script::clearMagicFunctions()
	{
		LuaController::pushNil();
		LuaController::setGlobal("onInit");
		LuaController::pushNil();
		LuaController::setGlobal("onUpdate");
		LuaController::pushNil();
		LuaController::setGlobal("onKeyPressed");
		LuaController::pushNil();
		LuaController::setGlobal("onKeyReleased");
	}
}