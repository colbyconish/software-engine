#include "pch.h"
#include "SWE/SWE.h"

namespace swe 
{
    Scene::Scene(std::string name)
        : name(name), ID(SceneManager::getSize())
    {
    }

    void Scene::addObject(std::shared_ptr<Object> obj)
    {
        objects.push_back(obj);
    }

    unsigned int Scene::getID() const
    {
        return ID;
    }
    std::string Scene::getName() const
    {
        return name;
    }
}