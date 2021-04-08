#include "Light.hpp"

namespace sckz
{
    void Light::CreateLight() { }

    void Light::DestroyLight() { }

    glm::vec3 Light::GetLocation() { return location; }

    glm::vec3 Light::GetColor() { return color; }

    glm::vec3 Light::GetAttenuation() { return attenuation; }

    void Light::SetLocation(float x, float y, float z)
    {
        location.x = x;
        location.y = y;
        location.z = z;
    }

    void Light::SetColor(float r, float g, float b)
    {
        color.x = r;
        color.y = g;
        color.z = b;
    }

    void Light::SetAttenuation(float a1, float a2, float a3)
    {
        attenuation.x = a1;
        attenuation.y = a2;
        attenuation.z = a3;
    }

} // namespace sckz