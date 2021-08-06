#pragma once
#include "../../Include.hpp"
#include "../../SckzExtras/LightSort.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "Entity.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Light.hpp"

namespace sckz
{
    class Entity
    {
    private:
        static const int MAX_LIGHTS = 4;

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

            alignas(16) glm::vec3 lightPosition[MAX_LIGHTS];

            alignas(16) float refractiveIndexRatio;
        };

        struct ComplexFragmentUniformBufferObject
        {
            alignas(16) glm::vec4 lightColor[MAX_LIGHTS];
            alignas(16) glm::vec4 attenuation[MAX_LIGHTS];
            alignas(16) glm::vec4 direction[MAX_LIGHTS];
            alignas(16) glm::vec4 cutoffs[MAX_LIGHTS];
            // Use Vec4 instead of vec3 for alignment, it takes up the same amount of bytes as aligning it properly and
            // this is easier
            alignas(16) glm::vec4 extras; // Reflectivity is s, shine damper is y, cutoff is z and outer cut off is w
            alignas(16) float reflectRefractFactor;
        };

    private:
        VkPhysicalDevice *     physicalDevice;
        VkQueue *              queue;
        Buffer *               hostLocalBuffer;
        VkDevice *             device;
        DescriptorPool *       descriptorPool;
        GraphicsPipeline *     pipeline;
        std::array<Image, 3> * textures;
        Image *                blankCubeImage;
        std::vector<Light *> * lights;

        Memory *        memory;
        VkFormat        format;
        VkCommandPool * commandPool;

        std::array<Buffer::SubBlock, 2> complexUniformBuffers;
        VkDescriptorSet                 complexDescriptorSet;
        Buffer::SubBlock                simpleUniformBuffer;
        VkDescriptorSet                 simpleDescriptorSet;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

        glm::mat4x4 currentModel;

        Camera cubeMapCamera;
        Fbo    reflectionMap;

        bool isCubeMap;

        float refractiveIndexRatio;
        float reflectRefractFactor;

        float shineDamper;
        float reflectivity;

    public:
        void CreateEntity(VkPhysicalDevice &     physicalDevice,
                          VkDevice &             device,
                          VkQueue &              queue,
                          Buffer &               hostLocalBuffer,
                          DescriptorPool &       pool,
                          GraphicsPipeline &     pipeline,
                          Memory &               memory,
                          VkFormat               format,
                          VkCommandPool &        commandPool,
                          bool                   isCubeMap,
                          std::array<Image, 3> & textures,
                          Image &                blankCubeImage);

        void DestroyEntity();

        void RebuildSwapResources(VkExtent2D swapChainExtent);
        void Update(Camera & camera);
        void UpdateCubeMap(Camera & cubeMapCamera);

    private:
        void CreateUniformBuffers();

    public:
        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetScale(float x, float y, float z);

        void LoadLights(std::vector<Light *> & lights);

        void SetShine(float reflectivity, float shineDamper);

        void SetReflectRefractValues(float refractiveIndexRatio, float reflectRefractFactor);

        Fbo & GetEnvironmentMapFBO();

        bool IsReflective();

        glm::vec3 GetLocation();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();

        VkDescriptorSet & GetSimpleDescriptorSet();
        VkDescriptorSet & GetComplexDescriptorSet();

        Camera & GetCubeMapCamera();
    };
} // namespace sckz