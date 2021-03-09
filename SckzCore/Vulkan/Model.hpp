#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Vertex.hpp"

namespace sckz
{
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class Model
    {
    private:
        Image texture;

        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;

        Buffer              vertexBuffer;
        Buffer              indexBuffer;
        std::vector<Buffer> uniformBuffers;

        std::vector<VkDescriptorSet> descriptorSets;
        std::vector<VkCommandBuffer> commandBuffers;

        VkDevice *                   device;
        VkPhysicalDevice *           physicalDevice;
        VkCommandPool *              commandPool;
        DescriptorPool *             descriptorPool;
        GraphicsPipeline *           pipeline;
        std::vector<VkFramebuffer> * swapChainFramebuffers;
        VkExtent2D                   swapChainExtent;
        uint32_t                     numSwapChainImages;
        VkQueue *                    queue;
        VkRenderPass *               renderPass;
        Memory *                     memory;
        bool                         hasCommandBuffer;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

        const char * textureFileName;
        const char * modelFileName;

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
                         uint32_t                     numSwapChainImages,
                         Memory &                     memory,
                         VkQueue &                    queue);
        void DestroyModel();

        void Update(uint32_t currentImage, Camera & camera);

        void DestroySwapResources();
        void CreateSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent);
        void CreateCommandBuffers();

    public:
        void LoadModel();
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffers();

        void CreateDescriptorSets();

    public:
        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetScale(float x, float y, float z);

        glm::vec3 GetLocation();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();

    public:
        std::vector<VkCommandBuffer> & GetCommandBuffers();
        Buffer                         GetIndexBuffer();
        Buffer                         GetVertexBuffer();
        std::vector<VkDescriptorSet> & GetDescriptorSets();
        uint32_t                       GetNumIndices();
    };
} // namespace sckz