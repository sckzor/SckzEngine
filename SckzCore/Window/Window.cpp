#include "Window.hpp"

namespace sckz
{
    // public member functions
    void Window::CreateWindow(const char * name, uint32_t width, uint32_t height)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(width, height, name, nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    }

    void Window::DestroyWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::Update() { glfwPollEvents(); }

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
} // namespace sckz