#include "Window.hpp"

namespace sckz
{
    // public member functions
    void Window::CreateWindow(const char * name, uint32_t width, uint32_t height, bool decorated)
    {
        glfwInit();

        initialWindowedWidth  = width;
        initialWindowedHieght = height;
        isFullScreen          = false;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_DECORATED, decorated ? GLFW_TRUE : GLFW_FALSE);

        window = glfwCreateWindow(width, height, name, nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
        glfwSetCursorPosCallback(window, MouseMoveCallback);
        glfwSetMouseButtonCallback(window, MousePressCallback);
        glfwSetKeyCallback(window, KeyPressCallback);
    }

    void Window::DestroyWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::Update() { glfwPollEvents(); }

    void Window::SetIcon(const char * fileName)
    {
        GLFWimage images[1];
        images[0].pixels = stbi_load(fileName, &images[0].width, &images[0].height, 0, 4); // rgba channels
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    }

    void Window::GoFullScreen()
    {
        isFullScreen = true;
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 1920, 1080, GLFW_DONT_CARE);
    }
    void Window::GoWindowed()
    {
        isFullScreen = false;
        glfwSetWindowMonitor(window, nullptr, 0, 0, initialWindowedWidth, initialWindowedHieght, GLFW_DONT_CARE);
    }

    bool Window::IsFullScreen() { return isFullScreen; }

    bool Window::QueryClose() { return glfwWindowShouldClose(window); }

    bool Window::QueryResize() { return resized; }

    GLFWwindow * Window::GetWindow() { return window; }

    VkExtent2D Window::GetSize()
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D size;
        size.width  = width;
        size.height = height;

        return size;
    }

    // public static fucntions
    void Window::FramebufferResizeCallback(GLFWwindow * window, int width, int height)
    {
        auto app     = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        app->resized = true;
    }

    void Window::KeyPressCallback(GLFWwindow * window, int key, int scanCode, int action, int mod)
    {
        auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        switch (action)
        {
            case GLFW_PRESS:
                app->keys[key] = true;
                break;
            case GLFW_RELEASE:
                app->keys[key] = false;
                break;
        }
    }

    bool Window::QueryKey(uint32_t key) { return keys[toupper(key)]; }

    void Window::MouseMoveCallback(GLFWwindow * window, double xpos, double ypos)
    {
        auto app    = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        app->mouseX = xpos;
        app->mouseY = ypos;
    }

    void Window::MousePressCallback(GLFWwindow * window, int button, int action, int mods)
    {
        auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

        if (action == GLFW_PRESS)
        {
            app->mouseButtons[button] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            app->mouseButtons[button] = false;
        }
    }

    glm::vec2 Window::GetMousePosition()
    {
        glm::vec2 location;
        location.x = (mouseX * 2) - GetSize().width;
        location.y = (mouseY * 2) - GetSize().height;
        return location;
    }

    bool Window::QueryMouseButton(uint32_t button) { return mouseButtons[button]; }
} // namespace sckz