#include "pch.h"
#include <SWE/Engine/window.h>
#include <SWE/Engine/application.h>
#include <future>

namespace swe
{
    Window::Window(int width, int height, const char *title,
                   GLFWmonitor *monitor, GLFWwindow *window,
                   bool resizable, Menu menu)
        : deltaTime(0.0f), lastFrame(0.0f), ID(nullptr), current_scene(nullptr)
    {
        //setting window hints
        //GLFW_SCALE_TO_MONITOR
        resizable ? glfwWindowHint(GLFW_RESIZABLE, GL_TRUE) : glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        ID = glfwCreateWindow(width, height, title, monitor, window);

        if (ID == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }

        setMenu(menu);

        glfwSetFramebufferSizeCallback(ID, framebuffer_size_callback);
        glfwSetCursorPosCallback(ID, mouse_callback);

#ifdef _WIN64
        SetWindowSubclass(getNativeID(), &Window::Subclassproc, (UINT_PTR) this, 0);
#endif
#ifdef __APPLE__
#endif
#ifdef __linux__
#endif
    }

    Window* Window::makeWindow(int width, int height, const char* title,
        GLFWmonitor* monitor, GLFWwindow* window,
        bool resizable, Menu menu)
    {
        return new Window(width, height, title, monitor, window, resizable, menu);
    }

    Window::~Window()
    {
        if (graphics_thread.joinable()) graphics_thread.join();
        glfwDestroyWindow(ID);
    }

    inline void Window::close()
    {
        glfwSetWindowShouldClose(ID, GLFW_TRUE);
    }

    void Window::makeCurrent()
    {
        glfwMakeContextCurrent(ID);
    }

    GLFWwindow* Window::getID()
    {
        return ID;
    }

    void Window::setAttr(windowAttr attr, bool set)
    {
        switch (attr)
        {
            case windowAttr::lockedCursor:
                glfwSetInputMode(ID, GLFW_CURSOR, set ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                break;
            default:
                std::cout << "No such attribute." << std::endl;
                break;
        }
    }

    void Window::setAttr(windowAttr attr, const char *set)
    {
        switch (attr)
        {
            case windowAttr::title:
                glfwSetWindowTitle(ID, set);
                break;
            default:
                std::cout << "No such attribute." << std::endl;
                break;
        }
    }

    void Window::drawBackground(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::drawBackground(glm::vec4 color)
    {
        glClearColor(color.x, color.y, color.z, color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(ID);
    }

    bool Window::isIconified() const
    {
        return glfwGetWindowAttrib(ID, GLFW_ICONIFIED);
    }

    void Window::processInput()
    {
        if (glfwGetKey(ID, GLFW_KEY_A) == GLFW_PRESS)
            glfwSetWindowShouldClose(ID, GLFW_TRUE);
    }

    inline void Window::swapBuffers()
    {
        glfwSwapBuffers(ID);
    }

    void Window::endLoop()
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    void Window::callMenuCallback(UINT_DATA id)
    {
        menuFunctions.at(id)((DYNAMIC_PTR)this);
    }

    Dimensions Window::getDimensions() const
    {
        Dimensions size;
        glfwGetWindowSize(ID, &size.width, &size.height);
        return size;
    }

    GLFWmonitor* Window::currentMonitor()
    {
        return glfwGetPrimaryMonitor();
    }

    void Window::thread_main(Window* win)
    {
        win->makeCurrent();
        Application::initGLAD(); 

        while (!win->shouldClose())
        {
            double start = glfwGetTime();
            win->renderScene();
            win->updateScene();
            win->swapBuffers();
            double elapsed = glfwGetTime() - start;

            //frame capping
            std::this_thread::sleep_for(std::chrono::milliseconds((int)((Application::getFpsCap() - elapsed)*1000)));
        }
    }

    void Window::startThread()
    {
        graphics_thread = std::thread(Window::thread_main, this);
    }

    void Window::stopThread()
    {
        graphics_thread.join();
    }

    void Window::loadScene(std::shared_ptr<Scene> scene)
    {
        current_scene = scene;
    }

    void Window::renderScene()
    {
        if (current_scene == nullptr)
            return;

        drawBackground(current_scene->background_color);
        current_scene->render(getDimensions());
    }

    void Window::updateScene()
    {
        if (current_scene == nullptr)
            return;

        current_scene->update();
    }

    void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Window::mouse_callback(GLFWwindow *window, double xpos, double ypos) {}

    //
    //Native Functions
    //
    
#ifdef _WIN64
    HWND Window::getNativeID()
    {
        if (win32_ID == NULL)
            setNativeValue();

        return win32_ID;
    }

    void Window::setNativeValue()
    {
        win32_ID = glfwGetWin32Window(ID);
    }

    void Window::setMenu(Menu menu)
    {
        if (win32_ID == nullptr)
            setNativeValue();

        menuFunctions = menu.getCallbacks();

        SetMenu(win32_ID, menu.getNativeMenu());
    }

    LRESULT Window::Subclassproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
    {
        Window* window = (Window*)uIdSubclass;
        switch(uMsg)
        {
        case WM_COMMAND:
            std::async([window](WPARAM wp) { window->callMenuCallback(wp);}, wParam);
            return 0;
        case WM_CLOSE:
            window->close();
            return 1;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }          
    }
#endif
#ifdef __APPLE__
#endif
#ifdef __linux__
#endif

} // END namespace swe