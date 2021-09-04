#pragma once
#include "../../Include.hpp"
#include "Bone.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "CubeMap.hpp"
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
        static const uint32_t SIZE_OF_VERTEX          = 16;
        static const uint32_t SIZE_OF_VERTEX_UNRIGGED = 8;

        std::array<Image, 3> textures;

        std::vector<Vertex>   vertices;
        std::vector<Bone>     bones;
        std::vector<uint32_t> indices;

        Buffer hostLocalBuffer;
        Buffer deviceLocalBuffer;
        Buffer stagingLocalBuffer;

        Memory * memory;

        Buffer::SubBlock * indexBuffer;
        Buffer::SubBlock * vertexBuffer;

        VkCommandBuffer commandBuffer;
        VkCommandBuffer cubeMapCommandBuffer;

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
        Image        blankTexture;
        Image        blankTextureCube;
        VkFormat     format;

        std::vector<Entity *> entities;

    public:
        void CreateModel(const char *       colorFileName,
                         const char *       normalFileName,
                         const char *       spacularFileName,
                         const char *       modelFileName,
                         VkCommandPool &    commandPool,
                         VkDevice &         device,
                         VkPhysicalDevice & physicalDevice,
                         GraphicsPipeline & pipeline,
                         DescriptorPool &   descriptorPool,
                         VkFormat           format,
                         Memory &           memory,
                         VkQueue &          queue);

        void DestroyModel();

        void Update(Camera & camera);
        void UpdateCubeCamera(Camera & cubeMapCamera);

        void RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent);

        Entity & CreateEntity(bool isReflectRefractive);

        void CreateCommandBuffer();
        void CreateCubeMapCommandBuffer();

    public:
        void CreateTexture(Image & image, const char * fileName);
        void LoadModel();
        void CreateVertexBuffer();
        void CreateIndexBuffer();

    public:
        VkCommandBuffer & GetCommandBuffer();
        VkCommandBuffer & GetCubeMapCommandBuffer();
        Buffer::SubBlock  GetIndexBuffer();
        Buffer::SubBlock  GetVertexBuffer();
        uint32_t          GetNumIndices();

        std::vector<Entity *> GetReflectiveEntities();

    private:
        void DestroySwapResources();
    };
} // namespace sckz