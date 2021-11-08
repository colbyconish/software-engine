#include "pch.h"
#include "SWE/SWE.h"


namespace swe
{
    Component::Component(compType type)
        : type(type)
    {
    }

    Component::~Component() {}

    compType Component::ClassType()
    {
        return compType::base;
    }

    compType Component::getType()
    {
        return type;
    }
} // namespace swe