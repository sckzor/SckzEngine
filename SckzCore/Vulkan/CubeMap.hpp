#pragma once
#include "../../Include.hpp"
#include "../Loaders/tiny_obj_loader.h"
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
    class CubeMap
    {
    private:
        struct SimpleVertexUniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 proj;
            alignas(16) glm::mat4 view[CUBEMAP_SIDES];
        };

        struct ComplexVertexUniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

    private:
        Image cubeMapTexture;

        VkDevice *         device;
        VkPhysicalDevice * physicalDevice;
        Memory *           memory;
        VkCommandPool *    pool;
        VkQueue *          queue;
        GraphicsPipeline * pipeline;
        DescriptorPool *   descriptorPool;
        Camera             reflectionCamera;

        float size;

        Buffer          hostLocalBuffer;
        Buffer          deviceLocalBuffer;
        Buffer          stagingLocalBuffer;
        VkDescriptorSet complexDescriptorSet;
        VkDescriptorSet simpleDescriptorSet;
        VkCommandBuffer complexCommandBuffer;
        VkCommandBuffer simpleCommandBuffer;

        Buffer::SubBlock * vertexBuffer;
        Buffer::SubBlock * indexBuffer;
        Buffer::SubBlock   simpleUniformBuffer;
        Buffer::SubBlock   complexUniformBuffer;

        std::vector<Vertex> vertices
            = { { { -1, -1, 1 }, { 0, 0, 0 }, { 0, 0 } }, { { -1, 1, 1 }, { 0, 0, 0 }, { 0, 0 } },
                { { -1, 1, -1 }, { 0, 0, 0 }, { 0, 0 } }, { { -1, -1, -1 }, { 0, 0, 0 }, { 0, 0 } },
                { { 1, 1, -1 }, { 0, 0, 0 }, { 0, 0 } },  { { 1, -1, -1 }, { 0, 0, 0 }, { 0, 0 } },
                { { 1, 1, 1 }, { 0, 0, 0 }, { 0, 0 } },   { { 1, -1, 1 }, { 0, 0, 0 }, { 0, 0 } } };

        std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3, 3, 2, 4, 3, 4, 5, 5, 4, 6, 5, 6, 7,
                                          7, 6, 1, 7, 1, 0, 3, 5, 7, 3, 7, 0, 4, 2, 1, 4, 1, 6 };

    public:
        void CreateCubeMap(const char *       front,
                           const char *       back,
                           const char *       up,
                           const char *       down,
                           const char *       right,
                           const char *       left,
                           VkDevice &         device,
                           VkPhysicalDevice & physicalDevice,
                           Memory &           memory,
                           VkCommandPool &    pool,
                           VkQueue &          queue,
                           GraphicsPipeline & graphicsPipeline,
                           DescriptorPool &   descriptorPool,
                           float              size);

        void DestroyCubeMap();

        VkCommandBuffer & GetSimpleCommandBuffer();
        VkCommandBuffer & GetComplexCommandBuffer();
        Image &           GetImage();

        void Update(Camera & camera);
        void UpdateCubeMap(Camera & cubeMapCamera);
        void RebuildSwapResources(DescriptorPool & descriptorPool);

    private:
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateSimpleCommandBuffer();
        void CreateComplexCommandBuffer();
        void CreateUniformBuffer();
    };
} // namespace sckz