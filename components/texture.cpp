/*#include <SWE/SWE.h>
#include "STB/stb_image.h"

unsigned int swe::Texture::texPosMin = GL_TEXTURE1;
unsigned int swe::Texture::texPosMax = GL_TEXTURE16;

//creates texture using the texture creation pos GL_TEXTURE0
swe::Texture::Texture(const char *fileLocation)
{
    glGenTextures(1, &ID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ID);

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
}

void swe::Texture::activateOn(unsigned int pos) const
{
    if (texPosMin <= pos && pos <= texPosMax)
    {
        glActiveTexture(pos);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
    else
        std::cout << "No Such Texture Position." << std::endl;
}*/