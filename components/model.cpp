#include "pch.h"
#include <SWE/Components/model.h>


namespace swe
{
    Model::Model()
    : Component() {}

    model_ptr Model::createModel()
    {
        return model_ptr(new Model());
    }

    Model::~Model(){}

    void Model::addMesh(mesh_ptr mesh)
    {
        meshes.push_back(mesh);
    }

    void Model::render(Shader *shader)
    {
        for (mesh_ptr mesh : meshes)
            mesh->draw(shader);
    }

    compType Model::getType() const
    {
        return compTypeFromTemplate<Model>{}.type;
    }

}// END namespace swe