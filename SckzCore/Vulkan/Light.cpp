#include "Light.hpp"

namespace sckz
{
    void Light::CreateLight(bool isSpotLight) { this->isSpotLight = isSpotLight; }

    void Light::DestroyLight() { }

    glm::vec3 Light::GetLocation() { return location; }

    glm::vec3 Light::GetColor() { return color; }

    glm::vec3 Light::GetDirection() { return direction; }

    glm::vec3 Light::GetAttenuation() { return attenuation; }

    float Light::GetCutoff() { return cutoff; }

    float Light::GetOuterCutoff() { return outerCutoff; }

    void Light::SetLocation(float x, float y, float z)
    {
        location.x = x;
        location.y = y;
        location.z = z;
    }

    void Light::SetDirection(float x, float y, float z)
    {
        direction.x = x;
        direction.y = y;
        direction.z = z;
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

    void Light::SetCutoff(float cutoff, float outerCutoff)
    {
        this->cutoff      = cutoff;
        this->outerCutoff = outerCutoff;
    }

    bool Light::IsSpotlight() { return isSpotLight; }
} // namespace sckz