#pragma once
#include "../../Include.hpp"

namespace sckz
{
    class Light
    {
    private:
        glm::vec3 location;
        glm::vec3 direction;
        glm::vec3 color;
        glm::vec3 attenuation = glm::vec3(1, 0, 0);
        float     cutoff;
        float     outerCutoff;
        bool      isSpotLight = false;

    public:
        void CreateLight(bool isSpotLight);
        void DestroyLight();

    public:
        glm::vec3 GetLocation();
        glm::vec3 GetDirection();
        glm::vec3 GetColor();
        glm::vec3 GetAttenuation();

        float GetCutoff();
        float GetOuterCutoff();

        void SetLocation(float x, float y, float z);
        void SetColor(float r, float g, float b);
        void SetCutoff(float cutoff, float outerCutoff);
        void SetDirection(float x, float y, float z);
        void SetAttenuation(float a1, float a2, float a3);

        bool IsSpotlight();
    };
} // namespace sckz