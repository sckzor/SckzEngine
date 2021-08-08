#include "Button.hpp"

namespace sckz
{
    void Button::CreateButton(Vulkan & vulkan, const char * texture)
    {
        this->vulkan = &vulkan;
        gui          = &vulkan.CreateGUI(texture, 2);
    }

    void Button::DestroyButton() { }

    bool Button::IsPressed(uint32_t button)
    {
        if (gui->GetScale().x > 0 && gui->GetScale().y > 0
            && vulkan->GetWindow().GetMousePosition().x >= gui->GetLocation().x - (gui->GetScale().x / 2)
            && vulkan->GetWindow().GetMousePosition().x <= gui->GetLocation().x + (gui->GetScale().x / 2)
            && vulkan->GetWindow().GetMousePosition().y >= gui->GetLocation().y - (gui->GetScale().y / 2)
            && vulkan->GetWindow().GetMousePosition().y <= gui->GetLocation().y + (gui->GetScale().y / 2))
        {
            gui->SetCurrentStage(1);
            gui->Update();

            if (vulkan->GetWindow().QueryMouseButton(button))
            {

                return true;
            }
            return false;
        }
        gui->Update();
        gui->SetCurrentStage(0);
        return false;
    }

    void Button::SetLocation(float x, float y) { gui->SetLocation(x, y); }
    void Button::SetScale(float x, float y) { gui->SetScale(x, y); }
    void Button::SetRotationPoint(float x, float y) { gui->SetRotationPoint(x, y); }
    void Button::SetRotation(float x) { gui->SetRotation(x); }

    glm::vec2 Button::GetLocation() { return gui->GetLocation(); }
    glm::vec2 Button::GetScale() { return gui->GetScale(); }
    glm::vec2 Button::GetRotationPoint() { return gui->GetRotationPoint(); }
    float     Button::GetRotation() { return gui->GetRotation(); }
} // namespace sckz