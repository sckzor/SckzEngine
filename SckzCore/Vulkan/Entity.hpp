#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "GraphicsPipeline.hpp"
#include "Model.hpp"

namespace sckz
{
    class Entity
    {
    private:
        std::vector<Buffer>          uniformBuffers;
        std::vector<VkDescriptorSet> descriptorSets;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

    private:
        void CreateUniformBuffers();
        void CreateDescriptorSets();

    public:
        void CreateEntity(VkPhysicalDevice & physicalDevice,
                          VkQueue &          queue,
                          Memory &           memory,
                          VkDevice &         device,
                          DescriptorPool &   pool,
                          GraphicsPipeline & pipeline,
                          uint32_t           numFrameBuffer);

        void RebuildSwapResources();
        void Update(uint32_t currentImage, Camera & camera);

        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetScale(float x, float y, float z);

        glm::vec3 GetLocation();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();

        std::vector<VkDescriptorSet> & GetDescriptorSets();
    };
} // namespace sckz