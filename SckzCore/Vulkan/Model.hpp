#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "Entity.hpp"
#include "Fbo.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Vertex.hpp"

namespace sckz
{
    class Model
    {
    private:
        std::array<Image, 3> textures;

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
        VkQueue *          queue;

        const char * colorFileName;
        const char * normalFileName;
        const char * spacularFileName;
        const char * modelFileName;

        std::vector<Entity *> entities;

    public:
        void CreateModel(const char *       colorFileName,
                         const char *       normalFileName,
                         const char *       spacularFileName,
                         const char *       modelFileName,
                         VkCommandPool &    commandPool,
                         VkDevice &         device,
                         VkPhysicalDevice & physicalDevice,
                         GraphicsPipeline * pipeline,
                         DescriptorPool &   descriptorPool,
                         Memory &           memory,
                         VkQueue &          queue);

        void DestroyModel();

        void Update(Camera & camera);

        void RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent);

        Entity & CreateEntity();

        void CreateCommandBuffer();

    public:
        void CreateTexture(Image & image, const char * fileName);
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