#include "pch.h"
#include <SWE/SWE.h>

#define TEXTURE_MAX 16

namespace swe
{
    int8_t Texture::texture_max = TEXTURE_MAX;
    uint16_t Texture::avail_textures = 0xFFFF;
    std::map<std::string, int8_t> Texture::active_textures = std::map<std::string, int8_t>();

    Texture::Texture(int8_t id, std::string filelocation, textureType type) :ID(id), type(type), filelocation(filelocation) {}

    Texture Texture::loadTexture(std::string fileLocation, textureType type)
    {
        //check for glad
        auto it = active_textures.find(fileLocation);
        if (it != active_textures.end())
            return Texture(active_textures[fileLocation], fileLocation, type);

        if (avail_textures <= 0)
        {
            std::cout << "Failed to load texture. Too many active textures." << std::endl;
            return Texture(-1, "", type);
        }

        uint16_t shift = avail_textures;
        int8_t offset = 0;
        while ((shift & 1) != 1)
        {
            shift >>= 1;
            offset++;
        }
        avail_textures = (1 << offset) ^ avail_textures;
        unsigned int id;

        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0+offset);
        glBindTexture(GL_TEXTURE_2D, id);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(fileLocation.c_str(), &width, &height, &nrChannels, 0);
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

        active_textures.emplace(fileLocation, offset);

        std::cout << fileLocation << " loaded into " << (int)offset << std::endl;
        Texture t = Texture(offset, fileLocation, type);
        return t;
    }

    void Texture::unload()
    {
        if (ID < 0)
        {
            std::cout << avail_textures << ", "<< (int)ID<< std::endl;
            return;
        }
        active_textures.erase(filelocation);
        avail_textures = (1 << ID) ^ avail_textures;
        std::cout << avail_textures;
    }
}