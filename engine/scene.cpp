#include "pch.h"
#include "SWE/SWE.h"

namespace swe 
{
    Scene::Scene() :background_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), currentShader(Shader::getDefaultShader())
                    , currentCamera(nullptr) {}

    scene_ptr Scene::createScene()
    {
        return scene_ptr(new Scene());
    }

    void Scene::addObject(std::shared_ptr<Object> obj)
    {
        objects.push_back(obj);
    }

    void Scene::render(Dimensions windowSize)
    {
        //if (currentCamera == nullptr)
            //return;
        for (object_ptr o : objects)
        {
            std::shared_ptr<Model> model = o->getComponent<Model>();
            if (model == nullptr)
                continue;
            model->render(windowSize, currentShader, o->getModelMatrix(), glm::mat4(1.0f)); //currentCamera->getViewMatrix()
        }
    }
}