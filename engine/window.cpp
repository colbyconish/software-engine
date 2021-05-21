#include "pch.h"
#include <SWE/SWE.h>

namespace swe
{
    Window *Window::currentContext = nullptr;
    std::vector<windowData> Window::windowRequests = std::vector<windowData>();
    std::vector<std::shared_ptr<Window>> Window::windows = std::vector<std::shared_ptr<Window>>();

    Window::Window(int width, int height, const char *title,
                   GLFWmonitor *monitor, GLFWwindow *window,
                   bool resizable, Menu menu) 
        : deltaTime(0.0f), lastFrame(0.0f), ID(nullptr)
    {
        if (!glfwReady)
        {
            std::cout << "GLFW not initialized." << std::endl;
            return;
        }

        //glfwWindowHint(GLFW_DECORATED, false);
        if (!resizable)
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        ID = glfwCreateWindow(width, height, title, monitor, window);

        if (ID == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        graphics_thread = std::thread(Window::thread_main, this);

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

    Window::~Window()
    {
        graphics_thread.join();
        glfwDestroyWindow(ID);
    }

    void Window::close()
    {
        glfwSetWindowShouldClose(ID, GLFW_TRUE);
    }

    void Window::makeCurrent()
    {
        currentContext = this;
        glfwMakeContextCurrent(ID);
        initGLAD();
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
                if (set)
                    glfwSetInputMode(ID, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                else
                    glfwSetInputMode(ID, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

    void Window::swapBuffers()
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

    Window* Window::getWindow(GLFWwindow* id)
    {
        for (std::shared_ptr<Window> win : windows)
            if (win->ID == id)
                return win.get();

        return nullptr;
    }

    void Window::AppMain()
    {
        while (windows.size() > 0)
        {
            //Fufill window requests
            while (windowRequests.size() > 0)
            {
                Window::createWindow(windowRequests.back());
                windowRequests.pop_back();
            }

            //Update existing windows
            for (int i = (int)windows.size() - 1; i > -1; i--)
            {
                if (!windows[i]->shouldClose())
                {
                    glfwWaitEvents();
                    windows[i]->endLoop();
                }
                else
                    windows.erase(windows.begin() + i);
            }
        }
        glfwTerminate();
    }

    void Window::thread_main(Window *win)
    {
        win->makeCurrent();

        while (!win->shouldClose())
        {
            win->drawBackground(0.1f, 0.4f, 0.3f);

            win->swapBuffers();
        }
    }

    void Window::createWindow(int width, int height, const char* title,
                                GLFWmonitor* monitor, GLFWwindow* window,
                                bool resizable, Menu menu)
    {
        windows.emplace_back(new Window(width, height, title, monitor, window, resizable, menu));
    }

    void Window::createWindow(windowData wd)
    {
        windows.emplace_back(new Window(wd.width, wd.height, wd.title, wd.monitor, wd.window, wd.resizable, wd.menu));
    }

    void Window::createWindowRequest(int width, int height, const char* title,
                                GLFWmonitor* monitor, GLFWwindow* window,
                                bool resizable, Menu menu)
    {
        windowRequests.emplace_back(windowData{width, height, title, monitor, window, resizable, menu});
    }

    void Window::createWindowRequest(windowData wd)
    {
        windowRequests.emplace_back(wd);
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
            std::async([window](WPARAM wp) { window->callMenuCallback(wp); }, wParam);
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