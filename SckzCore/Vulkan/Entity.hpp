#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "Entity.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"

namespace sckz
{
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class Entity
    {
    private:
        VkPhysicalDevice * physicalDevice;
        VkQueue *          queue;
        Memory *           memory;
        VkDevice *         device;
        DescriptorPool *   pool;
        GraphicsPipeline * pipeline;
        uint32_t           numFrameBuffers;
        Image *            texture;

        std::vector<Buffer>          uniformBuffers;
        std::vector<VkDescriptorSet> descriptorSets;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

    public:
        void CreateEntity(VkPhysicalDevice & physicalDevice,
                          VkDevice &         device,
                          VkQueue &          queue,
                          Memory &           memory,
                          DescriptorPool &   pool,
                          GraphicsPipeline & pipeline,
                          uint32_t           numFrameBuffers,
                          Image &            texture);

        void DestroyEntity();

        void RebuildSwapResources();
        void Update(uint32_t currentImage, Camera & camera);

    private:
        void CreateUniformBuffers();
        void CreateDescriptorSets();

    public:
        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetScale(float x, float y, float z);

        glm::vec3 GetLocation();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();

        std::vector<VkDescriptorSet> & GetDescriptorSets();
    };
} // namespace sckz