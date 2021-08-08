#pragma once
#include "../Include.hpp"
#include "../SckzCore/Vulkan/Gui.hpp"
#include "../SckzCore/Vulkan/Vulkan.hpp"
#include "../SckzCore/Window/Window.hpp"

namespace sckz
{
    class Button
    {
    private:
        Gui *    gui;
        Vulkan * vulkan;

    public:
        void CreateButton(Vulkan & vulkan, const char * texture);
        void DestroyButton();

        bool IsPressed(uint32_t button);

        void SetLocation(float x, float y);
        void SetScale(float x, float y);
        void SetRotationPoint(float x, float y);
        void SetRotation(float x);

        glm::vec2 GetLocation();
        glm::vec2 GetScale();
        glm::vec2 GetRotationPoint();
        float     GetRotation();
    };
} // namespace sckz