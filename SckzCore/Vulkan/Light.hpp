#pragma once
#include "../Include.hpp"

namespace sckz
{
    class Light
    {
    private:
        glm::vec3 location;
        glm::vec3 color;

    public:
        void CreateLight();
        void DestroyLight();

    public:
        glm::vec3 GetLocation();
        glm::vec3 GetColor();

        void SetLocation(float x, float y, float z);
        void SetColor(float r, float g, float b);
    };
} // namespace sckz