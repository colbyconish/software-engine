/*namespace swe
{
    Renderer::Renderer()
        : currentCamera(nullptr), currentShader(nullptr)
    {
        projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    }

    void Renderer::setCamera(std::shared_ptr<Camera> camera)
    {
        currentCamera = camera;
    }

    void Renderer::setShader(std::shared_ptr<Shader> shader)
    {
        currentShader = shader;
    }

    int Renderer::render(std::shared_ptr<Mesh> mesh, glm::mat4 model)
    {
        if (currentCamera == nullptr)
        {
            std::cout << "No camera assigned to renderer." << std::endl;
            return 1;
        }

        if (currentShader == nullptr)
        {
            std::cout << "No shader assigned to renderer." << std::endl;
            return 1;
        }

        if (mesh == nullptr)
        {
            std::cout << "No mesh to renderer." << std::endl;
            return 1;
        }

        currentShader->use();
        currentShader->setInt("ourTexture", 2); //change to objs texture
        currentShader->setMat4("model", model);
        currentShader->setMat4("projection", projectionMatrix);
        currentShader->setMat4("view", currentCamera->GetViewMatrix());

        mesh->bind();
        glDrawElements(GL_TRIANGLES, mesh->getNumIndis(), GL_UNSIGNED_INT, 0);
        return 0;
    }
} // END namespace swe*/