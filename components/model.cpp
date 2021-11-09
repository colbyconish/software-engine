#include "pch.h"
#include <SWE/Components/model.h>


namespace swe
{
    Model::Model()
    : Component(compType::model) {}

    model_ptr Model::createModel()
    {
        return model_ptr(new Model());
    }

    Model::~Model(){}

    void Model::addMesh(mesh_ptr mesh)
    {
        meshes.push_back(mesh);
    }

    void Model::render(Dimensions windowSize, Shader &shader, glm::mat4 model, glm::mat4 view)
    {
        for (mesh_ptr mesh : meshes)
            mesh->draw(windowSize, shader, model, view);
    }

    compType Model::ClassType()
    {
        return compType::model;
    }
}// END namespace swe