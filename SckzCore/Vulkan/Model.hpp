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

        Buffer hostLocalBuffer;
        Buffer deviceLocalBuffer;
        Buffer stagingLocalBuffer;

        Memory * memory;

        Buffer::SubBlock * indexBuffer;
        Buffer::SubBlock * vertexBuffer;

        VkCommandBuffer commandBuffer;

        VkDevice *         device;
        VkPhysicalDevice * physicalDevice;
        VkCommandPool *    commandPool;
        DescriptorPool *   descriptorPool;
        GraphicsPipeline * pipeline;
        VkFramebuffer *    framebuffer;
        VkExtent2D         swapChainExtent;
        VkQueue *          queue;
        VkRenderPass *     renderPass;
        bool               hasCommandBuffer;

        const char * textureFileName;
        const char * modelFileName;

        std::vector<Entity *> entities;

    public:
        void CreateModel(const char *       textureFileName,
                         const char *       modelFileName,
                         VkCommandPool &    commandPool,
                         VkRenderPass &     renderPass,
                         VkDevice &         device,
                         VkPhysicalDevice & physicalDevice,
                         VkFramebuffer &    swapChainFramebuffers,
                         GraphicsPipeline * pipeline,
                         DescriptorPool &   descriptorPool,
                         VkExtent2D         swapChainExtent,
                         Memory &           memory,
                         VkQueue &          queue);

        void DestroyModel();

        void Update(uint32_t currentImage, Camera & camera);

        void RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent);

        Entity & CreateEntity();

        void CreateCommandBuffer();

    public:
        void LoadModel();
        void CreateVertexBuffer();
        void CreateIndexBuffer();

    public:
        VkCommandBuffer & GetCommandBuffer();
        Buffer::SubBlock  GetIndexBuffer();
        Buffer::SubBlock  GetVertexBuffer();
        uint32_t          GetNumIndices();

    private:
        void DestroySwapResources();
    };
} // namespace sckz