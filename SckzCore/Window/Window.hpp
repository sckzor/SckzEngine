#pragma once
#include "../Include.hpp"

namespace sckz
{
    class Window
    {
    public:
        const static int total_keys = 349;

    private: // Private class members
        GLFWwindow * window;
        bool         resized          = false;
        bool         keys[total_keys] = { false };

    public: // Pulbic member functions
        void         CreateWindow(const char * name, uint32_t length, uint32_t width);
        void         DestroyWindow();
        void         Update();
        bool         QueryClose();
        bool         QueryResize();
        bool         QueryKey(uint32_t key);
        bool *       QueryKeys();
        GLFWwindow * GetWindow();
        VkExtent2D   GetSize();

    public: // Public static functions
        static void FramebufferResizeCallback(GLFWwindow * window, int width, int height);
        static void KeyPressCallback(GLFWwindow * window, int key, int scanCode, int action, int mod);
    };
} // namespace sckz