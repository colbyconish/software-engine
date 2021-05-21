#include "pch.h"
#include "SWE/SWE.h"


namespace swe
{
    Model::Model(std::vector<Mesh> meshes)
    : Component(compType::modelComp), meshes(meshes)
    {

    }

    Model::~Model(){}

    void Model::draw(Shader &shader)
    {
        for (Mesh mesh : meshes)
            mesh.draw(shader);
    }

    unsigned int Model::loadTexture(const char *fileLocation)
    {
        unsigned int id;
        
        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        
        int width, height, nrChannels;
        unsigned char *data = stbi_load(fileLocation, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, NULL);

        return id;
    }

    std::shared_ptr<Model> Model::shared_ptr()
    {
        return std::shared_ptr<Model>(this);
    }
}// END namespace swe