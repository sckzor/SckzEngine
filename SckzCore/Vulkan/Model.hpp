#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
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
        bool                         hasCommandBuffer;

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
                         GraphicsPipeline &           pipeline,
                         DescriptorPool &             descriptorPool,
                         VkExtent2D                   swapChainExtent,
                         uint32_t                     numSwapChainImages,
                         VkQueue &                    queue);
        void DestroyModel();

        void Update(uint32_t currentImage);

        void DestroySwapResources();
        void CreateSwapResources(GraphicsPipeline & pipeline,
                                 DescriptorPool &   descriptorPool,
                                 VkExtent2D         swapChainExtent);
        void CreateCommandBuffers();

    public:
        void LoadModel();
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffers();

        void CreateDescriptorSets();

    public:
        std::vector<VkCommandBuffer> & GetCommandBuffers();
        Buffer                         GetIndexBuffer();
        Buffer                         GetVertexBuffer();
        std::vector<VkDescriptorSet> & GetDescriptorSets();
        uint32_t                       GetNumIndices();
    };
} // namespace sckz