#include "pch.h"
#include "SWE/SWE.h"


namespace swe
{
    Component::Component(compType type)
        : type(type)
    {
    }

    Component::~Component() {}

    std::shared_ptr<Component> Component::shared_ptr()
    {
        std::cout << "Comp";
        return std::shared_ptr<Component>();
    }
} // namespace swe