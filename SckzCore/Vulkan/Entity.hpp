#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "Entity.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Light.hpp"

namespace sckz
{
    static const int MAX_LIGHTS = 4;

    struct SpecialVec3
    {
        glm::vec3 vec[MAX_LIGHTS];
    };

    struct VertexUniformBufferObject
    {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;

        alignas(16) glm::vec3 lightPosition[MAX_LIGHTS];
    };

    struct FragmentUniformBufferObject
    {
        alignas(16) glm::vec4 lightColor[MAX_LIGHTS];
        alignas(16) glm::vec4 attenuation[MAX_LIGHTS];
        // Use Vec4 instead of vec3 for alignment, it takes up the same amount of bytes as aligning it properly and
        // this is easier
        alignas(16) glm::vec2 reflectivity;
    };

    class Entity
    {
    private:
        VkPhysicalDevice *     physicalDevice;
        VkQueue *              queue;
        Buffer *               hostLocalBuffer;
        Buffer *               deviceLocalBuffer;
        VkDevice *             device;
        DescriptorPool *       pool;
        GraphicsPipeline *     pipeline;
        uint32_t               numFrameBuffers;
        Image *                texture;
        std::vector<Light *> * lights;

        std::vector<std::array<Buffer::SubBlock, 2>> uniformBuffers;
        std::vector<VkDescriptorSet>                 descriptorSets;

        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;

        float shineDamper;
        float reflectivity;

    public:
        void CreateEntity(VkPhysicalDevice & physicalDevice,
                          VkDevice &         device,
                          VkQueue &          queue,
                          Buffer &           hostLocalBuffer,
                          Buffer &           deviceLocalBuffer,
                          DescriptorPool &   pool,
                          GraphicsPipeline & pipeline,
                          uint32_t           numFrameBuffers,
                          Image &            texture);

        void DestroyEntity();

        void RebuildSwapResources();
        void Update(uint32_t currentImage, Camera & camera);

    private:
        void CreateUniformBuffers();
        void CreateDescriptorSets();

    public:
        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetScale(float x, float y, float z);

        void LoadLights(std::vector<Light *> & lights);

        void SetShine(float reflectivity, float shineDamper);

        glm::vec3 GetLocation();
        glm::vec3 GetRotation();
        glm::vec3 GetScale();

        std::vector<VkDescriptorSet> & GetDescriptorSets();
    };
} // namespace sckz