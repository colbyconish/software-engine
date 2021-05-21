#include "pch.h"
#include "framework.h"

#include <SWE\swe.h>


namespace swe {

    bool glfwReady = false;
    bool gladReady = false;

    glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);

    MenuCallback defaultCallback = [](DYNAMIC_PTR dp) {};

    void initGLFW()
    {
        if (!glfwReady)
        {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

            glfwReady = true;
        }
    }

    void initGLAD()
    {
        if(!gladReady)
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
                std::cout << "Failed to initialize GLAD" << std::endl;
    }
}