#pragma once
#include "../../Include.hpp"

namespace sckz
{
    class Camera
    {
    private:
        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 location;
        glm::vec3 rotation;

        float fov;
        float near, far;

        VkExtent2D extent;

    public:
        void CreateCamera(float fov, float near, float far, VkExtent2D extent);
        void UpdateExtent(VkExtent2D extent);
        void DestroyCamera();

        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetFOV(float fov);

    public:
        glm::mat4 GetView();
        glm::mat4 GetProjection();

        glm::vec3 GetRotation();
        glm::vec3 GetLocation();

    private:
        void UpdateMatrix();
    };
} // namespace sckz