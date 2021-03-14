#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "Entity.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Vertex.hpp"

namespace sckz
{
    class Model
    {
    private:
        Image texture;

        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;

        Buffer vertexBuffer;
        Buffer indexBuffer;
        // std::vector<Buffer> uniformBuffers;

        // std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkCommandBuffer> commandBuffers;

        VkDevice *                   device;
        VkPhysicalDevice *           physicalDevice;
        VkCommandPool *              commandPool;
        DescriptorPool *             descriptorPool;
        GraphicsPipeline *           pipeline;
        std::vector<VkFramebuffer> * swapChainFramebuffers;
        VkExtent2D                   swapChainExtent;
        VkQueue *                    queue;
        VkRenderPass *               renderPass;
        Memory *                     memory;
        bool                         hasCommandBuffer;

        // glm::vec3 location;
        // glm::vec3 rotation;
        // glm::vec3 scale;

        const char * textureFileName;
        const char * modelFileName;

        std::vector<Entity *> entities;

    public:
        void CreateModel(const char *                 textureFileName,
                         const char *                 modelFileName,
                         VkCommandPool &              commandPool,
                         VkRenderPass &               renderPass,
                         VkDevice &                   device,
                         VkPhysicalDevice &           physicalDevice,
                         std::vector<VkFramebuffer> & swapChainFramebuffers,
                         GraphicsPipeline *           pipeline,
                         DescriptorPool &             descriptorPool,
                         VkExtent2D                   swapChainExtent,
                         Memory &                     memory,
                         VkQueue &                    queue);

        void DestroyModel();

        void Update(uint32_t currentImage, Camera & camera);

        void RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent);

        Entity & CreateEntity();

        void CreateCommandBuffers();

    public:
        void LoadModel();
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        // void CreateUniformBuffers();

        // void CreateDescriptorSets();

    public:
        // void SetLocation(float x, float y, float z);
        // void SetRotation(float x, float y, float z);
        // void SetScale(float x, float y, float z);

        // glm::vec3 GetLocation();
        // glm::vec3 GetRotation();
        // glm::vec3 GetScale();

    public:
        std::vector<VkCommandBuffer> & GetCommandBuffers();
        Buffer                         GetIndexBuffer();
        Buffer                         GetVertexBuffer();
        // std::vector<VkDescriptorSet> & GetDescriptorSets();
        uint32_t GetNumIndices();

    private:
        void DestroySwapResources();
    };
} // namespace sckz