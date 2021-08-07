#pragma once
#include "../../Include.hpp"
#include "../Loaders/stb_image.h"

namespace sckz
{
    class Window
    {
    public:
        const static int total_keys          = 349;
        const static int total_mouse_buttons = 8;

    private: // Private class members
        GLFWwindow * window;
        bool         resized                           = false;
        bool         keys[total_keys]                  = { false };
        bool         mouseButtons[total_mouse_buttons] = { false };

        bool isFullScreen;

        uint32_t initialWindowedWidth;
        uint32_t initialWindowedHieght;

        double mouseX;
        double mouseY;

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

        glm::vec2 GetMousePosition();
        bool      QueryMouseButton(uint32_t button);

        bool IsFullScreen();

    public: // Public static functions
        static void FramebufferResizeCallback(GLFWwindow * window, int width, int height);
        static void KeyPressCallback(GLFWwindow * window, int key, int scanCode, int action, int mod);
        static void MouseMoveCallback(GLFWwindow * window, double xpos, double ypos);
        static void MousePressCallback(GLFWwindow * window, int button, int action, int mods);
    };
} // namespace sckz