#pragma once
#include "../../Include.hpp"
#include "Camera.hpp"
#include "Fbo.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"

namespace sckz
{

    class ParticleSystem
    {
    private:
        struct StaticVertexUniformBufferObject
        {
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        struct PerParticleVertexUniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::vec4 stageData;
        };

        struct Particle
        {
            Buffer::SubBlock uniformBuffer;
            VkDescriptorSet  descriptorSet;
            VkCommandBuffer  particleBuffer;
            glm::vec3        location;
            glm::vec3        velocity;
            float            rotation;
            float            scale;
            float            elapsedTime = 0;
        };

    private:
        std::vector<Particle> particles;
        VkDevice *            device;
        VkPhysicalDevice *    physicalDevice;
        VkCommandPool *       commandPool;
        DescriptorPool *      descriptorPool;
        GraphicsPipeline *    pipeline;
        VkQueue *             queue;
        Memory *              memory;
        bool                  updated = false;

        const float GRAVITY = -5.1f; //

        VkCommandBuffer  commandBuffer;
        Buffer           hostLocalBuffer;
        Buffer::SubBlock staticUniformBuffer;

        const char * textureFileName;
        Image        texture;

        uint32_t vStages;
        uint32_t hStages;
        uint32_t totalStages;

        bool rendering;

        float lifeLength;

    public:
        void CreateParticleSystem(const char *       textureFileName,
                                  uint32_t           vStages,
                                  uint32_t           hStages,
                                  uint32_t           totalStages,
                                  VkCommandPool &    commandPool,
                                  VkDevice &         device,
                                  VkPhysicalDevice & physicalDevice,
                                  GraphicsPipeline & pipeline,
                                  DescriptorPool &   descriptorPool,
                                  Memory &           memory,
                                  VkQueue &          queue,
                                  uint32_t           maxParticles);

        void Update(Camera & camera, float delataTime);

        void DestroyParticleSystem();

        void AddParticle(float lx, float ly, float lz, float vx, float vy, float vz);

    private:
        void CreateUniformBuffers();
        void CreateCommandBuffer();
        void RebuildCommandBuffer();

        void DestroySwapResources();
        void UpdateMvpMatrix(Particle & particle, StaticVertexUniformBufferObject & Subo);

    public:
        void RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent);

        VkCommandBuffer & GetCommandBuffer();
    };
} // namespace sckz