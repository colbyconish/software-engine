#include "pch.h"
#include "SWE/SWE.h"


namespace swe
{
    Object2D::Object2D(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
        : Object(pos, rot, glm::vec3(s.x, s.y, 1))
    {
    }

    std::shared_ptr<Object> Object2D::createObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 s)
    {
        return std::shared_ptr<Object>(std::move(new Object2D(pos, rot, s)));
    }

    Object2D::~Object2D()
    {
    }

    bool Object2D::isUI() { return false; }
} // END namespace swe