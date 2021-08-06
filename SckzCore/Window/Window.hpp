#pragma once
#include "../../Include.hpp"
#include "../Loaders/stb_image.h"

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

        bool isFullScreen;

        uint32_t initialWindowedWidth;
        uint32_t initialWindowedHieght;

    public: // Pulbic member functions
        void         CreateWindow(const char * name, uint32_t length, uint32_t width, bool decorated);
        void         DestroyWindow();
        void         Update();
        void         SetIcon(const char * fileName);
        bool         QueryClose();
        bool         QueryResize();
        bool         QueryKey(uint32_t key);
        bool *       QueryKeys();
        GLFWwindow * GetWindow();
        VkExtent2D   GetSize();
        void         GoFullScreen();
        void         GoWindowed();

        bool IsFullScreen();

    public: // Public static functions
        static void FramebufferResizeCallback(GLFWwindow * window, int width, int height);
        static void KeyPressCallback(GLFWwindow * window, int key, int scanCode, int action, int mod);
    };
} // namespace sckz