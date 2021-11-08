#include "pch.h"
#include "framework.h"

#define STB_IMAGE_IMPLEMENTATION
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

    int initGLAD()
    {
        if (!glfwReady) initGLFW();//throw warning

        GLFWwindow *temp = glfwGetCurrentContext();
        if (temp == nullptr)
        {
            std::cout << "There must be a current context to initialize glad." << std::endl;//throw error
            return 0;
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            std::cout << "Failed to initialize GLAD" << std::endl;//throw error

        return 1;
    }
}