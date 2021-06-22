#pragma once
#include "../Include.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "Fbo.hpp"
#include "GraphicsPipeline.hpp"
#include "HelperMethods.hpp"
#include "Image.hpp"
#include "Model.hpp"
#include "ParticleSystem.hpp"

namespace sckz
{
    class Scene
    {
    private:
        VkPhysicalDevice * physicalDevice; // Stays
        VkDevice *         device;         // Stays
        VkQueue *          graphicsQueue;  // Stays

        VkCommandBuffer primaryCmdBuffer; // Duplicated

        // Image         renderedImage; // Stays
        // VkFramebuffer renderedImageFrameBuffer;

        // VkRenderPass renderPass;      // Duplicated
        VkExtent2D swapChainExtent; // Stays

        DescriptorPool descriptorPool; // Duplicated
        VkCommandPool  commandPool;    // Duplicated

        // Image colorImage; // Duplicated
        Image depthImage; // Duplicated

        Fbo fboImage;

        VkFormat format;

        Memory memory;

        GraphicsPipeline particlePipeline;

        std::vector<GraphicsPipeline *> pipelines; // Goes
        std::vector<Model *>            models;    // Goes
        std::vector<Camera *>           cameras;   // Goes
        std::vector<Light *>            lights;
        std::vector<ParticleSystem *>   particleSystems;

        VkFence inFlightFence;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // Goes
        VkSampleCountFlagBits maxMsaaSamples;
        int32_t               targetMsaaSamples = 1;

        bool isUpdated;

    public:
        void CreateScene(VkPhysicalDevice & physicalDevice,
                         VkDevice &         device,
                         VkQueue &          graphicsQueue,
                         VkFormat &         imageFormant,
                         VkExtent2D         swapChainExtent);

        void DestroyScene();

        void RebuildSwapResources(VkExtent2D newExtent);

    private:
        void CreateImage();
        void CreateRenderPass();
        void CreateCommandPool();
        void CreateCommandBuffer(); // Missing implementation
        void CreateColorResources();
        void CreateDepthResources();
        void CreateFramebuffers();
        void CreateSyncObjects();

        void RebuildCommandBuffer(); // Missing implementation

    private:
        void DestroyRenderPass();
        void DestroyCommandPool();
        void DestroyCommandBuffer(); // Missing implementation
        void DestroyColorResources();
        void DestroyDepthResources();
        void DestroyFramebuffers();
        void DestroySyncObjects();

    private:
        void DestroySwapResources();

    private:
        VkSampleCountFlagBits GetTargetSampleCount(int32_t targetSampleCount);

    public:
        void Render(Camera & camera, float deltaTime);
        void SetMSAA(int32_t targetMsaaSamples);

        Image & GetRenderedImage();
        bool    IsUpdated();

    public:
        GraphicsPipeline & CreatePipeline(const char * vertexFile, const char * fragmentFile);

        Model & CreateModel(const char *       modelFile,
                            const char *       colorFile,
                            const char *       normalFile,
                            const char *       specularFile,
                            GraphicsPipeline & pipeline);

        Camera &         CreateCamera(float fov, float near, float far);
        Entity &         CreateEntity(Model & model);
        Light &          CreateLight();
        ParticleSystem & CreateParticleSystem(uint32_t     numStages,
                                              const char * texture,
                                              uint32_t     hStages,
                                              uint32_t     vStages,
                                              uint32_t     totalStages);
    };
} // namespace sckz