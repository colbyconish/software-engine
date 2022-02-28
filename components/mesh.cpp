#include "pch.h"
#include <SWE/Components/mesh.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <SWE/Engine/error.h>

#define POV 45.0f

namespace swe
{
    Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indis, Texture texture)
        : vertices(verts), indices(indis), texture(texture)
    {
        generateBuffers();
    }

    mesh_ptr Mesh::createMesh(std::vector<Vertex> verts, std::vector<uint32_t> indis, Texture texture)
    {
        if (texture.ID == -1)
            std::cout << "Mesh does not have texture." << std::endl;

        return mesh_ptr(new Mesh(verts, indis, texture));
    }

    void Mesh::generateBuffers()
    {
        //initGLAD();
        glGenVertexArrays(1, &VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);

        unsigned int EBO;
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

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

    void Mesh::draw(Dimensions windowSize, Shader &shader, glm::mat4 model, glm::mat4 view) const
    {
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", glm::perspective(glm::radians(POV), (float)windowSize.width / (float)windowSize.height, 0.1f, 1000.0f));

        if (texture.ID != -1)
            shader.setInt("material.diffuse", texture.ID);     

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, (const GLvoid *) 0);
        glBindVertexArray(0);
    }
} // namespace swe