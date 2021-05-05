#pragma once
#include "../Include.hpp"
#include "Camera.hpp"
#include "DescriptorPool.hpp"
#include "GraphicsPipeline.hpp"
#include "HelperMethods.hpp"
#include "Image.hpp"
#include "Model.hpp"

namespace sckz
{
    class Scene
    {
    private:
        VkPhysicalDevice * physicalDevice; // Stays
        VkDevice *         device;         // Stays
        VkQueue *          graphicsQueue;  // Stays

        VkCommandBuffer primaryCmdBuffer; // Duplicated

        Image         renderedImage; // Stays
        VkFramebuffer renderedImageFrameBuffer;

        VkRenderPass renderPass;      // Duplicated
        VkExtent2D   swapChainExtent; // Stays

        DescriptorPool descriptorPool; // Duplicated
        VkCommandPool  commandPool;    // Duplicated

        Image colorImage; // Duplicated
        Image depthImage; // Duplicated

        Memory memory;

        std::vector<GraphicsPipeline *> pipelines; // Goes
        std::vector<Model *>            models;    // Goes
        std::vector<Camera *>           cameras;   // Goes
        std::vector<Light *>            lights;

        VkSemaphore imageAvailableSemaphore; // Duplicated
        VkSemaphore renderFinishedSemaphore; // Duplicated
        VkFence     inFlightFence;           // Duplicated
        VkFence     imagesInFlight;          // Duplicated

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // Goes
        VkSampleCountFlagBits maxMsaaSamples;
        int32_t               targetMsaaSamples = 1;

    public:
        void CreateScene(VkPhysicalDevice & physicalDevice,
                         VkDevice &         device,
                         VkQueue &          graphicsQueue,
                         VkFormat &         imageFormant);

        void DestroyScene();

        void RebuildSwapResources();

    private:
        void CreateImageViews();
        void CreateRenderPass();
        void CreateCommandPool();
        void CreateCommandBuffer(); // Missing implementation
        void CreateColorResources();
        void CreateDepthResources();
        void CreateFramebuffers();
        void CreateSyncObjects();

    private:
        void DestroyImageViews();
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
        void Render(Camera & camera);
        void SetMSAA(int32_t targetMsaaSamples);

    public:
        GraphicsPipeline & CreatePipeline(const char * vertexFile, const char * fragmentFile);
        Model &            CreateModel(const char * modelFile, const char * textureFile, GraphicsPipeline & pipeline);
        Camera &           CreateCamera(float fov, float near, float far);
        Entity &           CreateEntity(Model & model);
        Light &            CreateLight();
    };
} // namespace sckz