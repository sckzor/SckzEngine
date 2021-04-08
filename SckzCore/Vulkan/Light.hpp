#pragma once
#include "../Include.hpp"

namespace sckz
{
    class Light
    {
    private:
        glm::vec3 location;
        glm::vec3 color;
        glm::vec3 attenuation = glm::vec3(1, 0, 0);

    public:
        void CreateLight();
        void DestroyLight();

    public:
        glm::vec3 GetLocation();
        glm::vec3 GetColor();
        glm::vec3 GetAttenuation();

        void SetLocation(float x, float y, float z);
        void SetColor(float r, float g, float b);
        void SetAttenuation(float a1, float a2, float a3);
    };
} // namespace sckz