#include "pch.h"
#include "SWE/SWE.h"


namespace swe
{
    Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indis, Texture text)
        : vertices(verts), indices(indis), texture(text)
    {
        generateBuffers();
    }

    void Mesh::generateBuffers()
    {
        glGenVertexArrays(1, &VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);

        unsigned int EBO;
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    size_t Mesh::getNumIndis()
    {
        return indices.size();
    }

    size_t Mesh::getNumVerts()
    {
        return vertices.size();
    }

    void Mesh::draw(Shader &shader) const
    {
        shader.setInt("material.diffuse", texture.ID);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }
} // namespace swe