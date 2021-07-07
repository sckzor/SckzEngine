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
        struct VertexUniformBufferObject
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

        Buffer          hostLocalBuffer;
        Buffer          deviceLocalBuffer;
        Buffer          stagingLocalBuffer;
        VkDescriptorSet descriptorSet;
        VkCommandBuffer commandBuffer;

        Buffer::SubBlock * vertexBuffer;
        Buffer::SubBlock * indexBuffer;
        Buffer::SubBlock   uniformBuffer;

        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;

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
                           DescriptorPool &   descriptorPool);

        void DestroyCubeMap();

        VkCommandBuffer & GetCommandBuffer();

        void Update(glm::vec3 location, Camera & camera);

    private:
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateCommandBuffer();
        void CreateUniformBuffer();

        void LoadModel();
    };
} // namespace sckz