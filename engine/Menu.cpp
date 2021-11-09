#include "pch.h"
#include <SWE/Engine/menu.h>
#include <map>

namespace swe
{
	size_t MenuTab::tabCount = 0;

	MenuCallback defaultCallback = [](DYNAMIC_PTR dp) {};

	MenuTab::MenuTab(std::string name, MenuCallback callBack, tabType type) 
		: name(name), type(type), callBack(callBack), ID(tabCount++) {}

	MenuTab::MenuTab(std::string name, std::vector<MenuTab> tabs)
		: name(name), tabs(tabs), type(tabType::Text), ID(tabCount++), callBack(defaultCallback) {}

	void MenuTab::getCallbacks(std::map<WPARAM, MenuCallback>* map) const
	{
		if (tabs.size() == 0)
			(*map)[ID] = callBack;
		else
			for (MenuTab t : tabs)
				t.getCallbacks(map);
	}

	void MenuTab::addTab(MenuTab tab)
	{
		if (type == tabType::Separator)
		{
			/* Throw error. Separators cannot enclose other tabs*/
			return;
		}
		tabs.push_back(tab);
	}

	size_t MenuTab::size() const
	{
		return tabs.size();
	}

	unsigned long long MenuTab::getID() const
	{
		return ID;
	}

	Menu::Menu() {}

	Menu::Menu(std::vector<MenuTab> tabs) : tabs(tabs) {}

	std::map<WPARAM, MenuCallback> Menu::getCallbacks() const
	{
		std::map<WPARAM, MenuCallback> map;
		for (MenuTab t : tabs)
			t.getCallbacks(&map);
		return map;
	}

	void Menu::addTab(MenuTab tab)
	{
		tabs.push_back(tab);
	}

	size_t Menu::size() const
	{
		return tabs.size();
	}

	//
	//Native Functions
	//

#ifdef _WIN64
	UINT_PTR MenuTab::getMenu()
	{
		if (type == tabType::Separator)
			return NULL;

		menu = CreateMenu();
		for (MenuTab t : tabs)
			if (t.type == tabType::Separator)
				AppendMenuA(menu, MF_SEPARATOR, NULL, "");
			else if (t.size() == 0)
				AppendMenuA(menu, MF_STRING, t.getID(), t.name.c_str());
			else
				AppendMenuA(menu, MF_POPUP, t.getMenu(), t.name.c_str());

		return (UINT_PTR) menu;
	}

	HMENU Menu::getNativeMenu() const
	{
		HMENU menu = CreateMenu();

		for (MenuTab t : tabs)
			if (t.size() == 0)
				AppendMenuA(menu, MF_STRING, t.getID(), t.name.c_str());
			else
				AppendMenuA(menu, MF_POPUP, t.getMenu(), t.name.c_str());
		
		return menu;
	}
#endif
#ifdef __APPLE__
#endif
#ifdef __linux__
#endif
}