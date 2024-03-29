#include "pch.h"
#include <SWE/Engine/window.h>
#include <SWE/Engine/error.h>
#include <SWE/Components/model.h>
#include <SWE/Objects/object.h>
#include <SWE/Components/light.h>

#define FOV 45.0f
#define NEAR 0.1f
#define FAR 1000.0f

namespace swe 
{
    Scene::Scene() :background_color(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), currentShader(Shader::getDefaultShader()),
                    lightShader(Shader::getLightShader()), currentCamera(nullptr) {}

    Scene::~Scene()
    {
        for (object_ptr o : objects)
            o->parent = nullptr;

        for (object_ptr o : lights)
            o->parent = nullptr;
    }

    scene_ptr Scene::createScene()
    {
        return scene_ptr(new Scene());
    }

    void Scene::addObject(std::shared_ptr<Object> obj)
    {
        objects.push_back(obj);
        obj->parent = this;
    }

    void Scene::addLight(std::shared_ptr<Object> obj)
    {
        lights.push_back(obj);
        obj->parent = this;
    }

    void Scene::render(Dimensions windowSize)
    {
        if (currentCamera == nullptr)
            return;

        currentShader.use();

        void* light;
        uint32_t d_lights = 0;
        uint32_t p_lights = 0;
        uint32_t s_lights = 0;

        std::string location;
        for (object_ptr o : lights)
        {
            light_ptr light_comp = o->getComponent<Light>();
            if (light_comp == nullptr)
                continue;

            switch (light_comp->type)
            {
            case lightType::directional:
                light = light_comp.get();
                location = std::string("d_lights[" + std::to_string(d_lights)) + "]";
                currentShader.setDLight(location, (DirectionalLight*) light);
                d_lights++;
                break;  
            case lightType::point:
                light = light_comp.get();
                location = std::string("p_lights[" + std::to_string(p_lights)) + "]";
                currentShader.setPLight(location, (PointLight*)light);
                p_lights++;
                break;
            case lightType::spot:
                light = light_comp.get();
                location = std::string("s_lights[" + std::to_string(s_lights)) + "]";
                currentShader.setSLight(location, (SpotLight*)light);
                s_lights++;
                break;
            default:
                Error err = Error("Light type not handled",errorLevel::Error, __SOURCELOCATION__);
            }
        }

        for (object_ptr o : objects)
        {
            if (!(o->visible))
                continue;

            model_ptr model = o->getComponent<Model>();
            if (model == nullptr)
                continue;

            currentShader.setMat4("model", o->getComponent<Transform>()->getModelMatrix());
            currentShader.setMat4("view", currentCamera->getViewMatrix());
            currentShader.setVec3("viewPos", *currentCamera->getComponent<Transform>()->position);
            currentShader.setMat4("projection", glm::perspective(glm::radians(FOV), (float)windowSize.width / (float)windowSize.height, NEAR, FAR));

            model->render(&currentShader);
        }

        //debug code
        lightShader.use();
        for (object_ptr o : lights)
        {
            if (!(o->visible))
                continue;

            model_ptr model = o->getComponent<Model>();
            if (model == nullptr)
                continue;

            light_ptr light = o->getComponent<Light>();

            lightShader.setVec4("color", light->color->x, light->color->y, light->color->z, 0.0f);
            lightShader.setMat4("model", o->getComponent<Transform>()->getModelMatrix());
            lightShader.setMat4("view", currentCamera->getViewMatrix());
            lightShader.setVec3("viewPos", *currentCamera->getComponent<Transform>()->position);
            lightShader.setMat4("projection", glm::perspective(glm::radians(FOV), (float)windowSize.width / (float)windowSize.height, NEAR, FAR));

            model->render(&lightShader);
        }
    }

    void Scene::update()
    {
        for (object_ptr o : objects)
            o->update();

        for (object_ptr o : lights)
            o->update();
    }

    void Scene::close()
    {
        for (object_ptr o : objects)
            o->close();

        for (object_ptr o : lights)
            o->close();
    }

    Window* Scene::getWindow()
    {
        return parent;
    }

    Window* Object::getWindow()
    {
        if (parent == nullptr)
            return nullptr;
        return parent->getWindow();
    }

    
}