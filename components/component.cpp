#include "pch.h"
#include <SWE/Components/component.h>
#include <SWE/Engine/error.h>

namespace swe
{
    Component::Component() :parent(nullptr) {}

    Component::~Component() {}

    void Component::init() {}

    void Component::update() {}

    void Component::close() {}

    compType Component::getType() const
    {
        return compTypeFromTemplate<Component>{}.type;
    }

    compType Component::stringToCompType(std::string type)
    {
        if (type == "base" || type == "Base") return compType::base;
        else if (type == "transform" || type == "Transform") return compType::transform;
        else if (type == "model" || type == "Model") return compType::model;
        else if (type == "script" || type == "Script") return compType::script;
        else if (type == "light" || type == "Light") return compType::light;

        Error err = Error("compType string not supported.", errorLevel::Error, __SOURCELOCATION__);
        return compType::null;
    }
} // namespace swe