#include "pch.h"
#include "SWE/SWE.h"


namespace swe
{
    std::vector<std::shared_ptr<Scene>> SceneManager::scenes = std::vector<std::shared_ptr<Scene>>();

    void SceneManager::addScene(std::shared_ptr<Scene> scene)
    {
        scenes.push_back(scene);
    }

    unsigned int SceneManager::getSize()
    {
        return (unsigned int) scenes.size();
    }
} // END namespace swe