#include "Light.hpp"

namespace sckz
{
    void Light::CreateLight() { }

    void Light::DestroyLight() { }

    glm::vec3 Light::GetLocation() { return location; }

    glm::vec3 Light::GetColor() { return color; }

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

} // namespace sckz