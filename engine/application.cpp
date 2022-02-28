#include "pch.h"
#include <SWE/Engine/application.h>
#include <SWE/Engine/luaController.h>
#include <iostream>


namespace swe
{
    bool Application::glfwReady = false;
    float Application::fpsCap = (1.0f / 60.0f);

    std::vector<windowData> Application::windowRequests = std::vector<windowData>();
    std::vector<std::shared_ptr<Window>> Application::windows = std::vector<std::shared_ptr<Window>>();

    float Application::getFpsCap()
    {
        return fpsCap;
    }

    void Application::Init()
    {
        initGLFW();
        LuaController::init();
    }

    void Application::Main()
    {
        if (!glfwReady)
        {
            std::cout << "GLFW not initialized." << std::endl;
            return;
        }

        //Sart all graphics threads
        glfwMakeContextCurrent(NULL);
        for (window_ptr win : windows)
            win->startThread();

        while (windows.size() > 0)
        {
            //Fufill window requests
            while (windowRequests.size() > 0)
            {
                createWindow(windowRequests.back());
                windowRequests.pop_back();
            }

            //Update existing windows
            for (int i = (int)windows.size() - 1; i > -1; i--)
            {
                if (!windows[i]->shouldClose())
                {
                    glfwWaitEvents();
                    windows[i]->processInput();
                    windows[i]->endLoop();
                }
                else
                {
                    windows[i]->stopThread();
                    windows.erase(windows.begin() + i);
                }
            }
        }

        LuaController::close();
        glfwTerminate();
    }

    window_ptr Application::getWindow(GLFWwindow* id)
    {
        for (std::shared_ptr<Window> win : windows)
            if (win->getID() == id)
                return win;

        return nullptr;
    }

    window_ptr Application::createWindow(int width, int height, const char* title,
        GLFWmonitor* monitor, GLFWwindow* window,
        bool resizable, Menu menu)
    {
        if (!glfwReady)
        {
            std::cout << "GLFW not initialized." << std::endl;
            return nullptr;
        }

        windows.emplace_back(Window::makeWindow(width, height, title, monitor, window, resizable, menu));
        return windows.back();
    }

    window_ptr Application::createWindow(windowData wd)
    {
        if (!glfwReady)
        {
            std::cout << "GLFW not initialized." << std::endl;
            return nullptr;
        }

        windows.emplace_back(Window::makeWindow(wd.size.width, wd.size.height, wd.title, wd.monitor, wd.window, wd.resizable, wd.menu));
        return windows.back();
    }

    void Application::requestWindow(int width, int height, const char* title,
        GLFWmonitor* monitor, GLFWwindow* window,
        bool resizable, Menu menu)
    {
        windowRequests.emplace_back(windowData{ {width, height}, title, monitor, window, resizable, menu });
    }

    void Application::requestWindow(windowData wd)
    {
        windowRequests.emplace_back(wd);
    }

    void Application::initGLFW()
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

    int Application::initGLAD()
    {
        if (!glfwReady) initGLFW();//throw warning

        GLFWwindow* temp = glfwGetCurrentContext();
        if (temp == nullptr)
        {
            std::cout << "There must be a current context to initialize glad." << std::endl;//throw error
            return 0;
        }

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            std::cout << "Failed to initialize GLAD" << std::endl;//throw error

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        return 1;
    }
}