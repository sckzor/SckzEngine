#pragma once
#include "../Include.hpp"

namespace sckz {
    class Window {
    private: // Private class members
        GLFWwindow * window;
        bool         resized = false;

    public: // Pulbic member functions
        void         CreateWindow(const char * name, uint32_t length, uint32_t width);
        void         DestroyWindow();
        void         Update();
        bool         QueryClose();
        bool         QueryResize();
        GLFWwindow * GetWindow();
        VkExtent2D   GetSize();

    public: // Public static functions
        static void  FramebufferResizeCallback(GLFWwindow * window, int width, int height);
    
    };
}