#pragma once
#include "../../Include.hpp"
#include "BoneInfo.hpp"
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
        std::array<Image, 3> textures;

        std::vector<Vertex>             vertices;
        std::vector<uint32_t>           indices;
        std::map<std::string, BoneInfo> boneMap; //
        uint32_t                        boneCount = 0;

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

        Entity & CreateEntity(bool isReflectRefractive, bool isAnimated);

        void CreateCommandBuffer();
        void CreateCubeMapCommandBuffer();

    public:
        void CreateTexture(Image & image, const char * fileName);
        void LoadModel();
        void ExtractBoneWeightForVertices(aiMesh * mesh, const aiScene * scene);

        void CreateVertexBuffer();
        void CreateIndexBuffer();

    public:
        VkCommandBuffer & GetCommandBuffer();
        VkCommandBuffer & GetCubeMapCommandBuffer();
        Buffer::SubBlock  GetIndexBuffer();
        Buffer::SubBlock  GetVertexBuffer();
        uint32_t          GetNumIndices();

        std::map<std::string, BoneInfo> & GetBoneMap();
        uint32_t                          GetBoneCount();

        std::vector<Entity *> GetReflectiveEntities();

    private:
        void DestroySwapResources();
    };
} // namespace sckz