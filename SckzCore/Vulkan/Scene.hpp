#pragma once
#include "../../Include.hpp"
#include "Camera.hpp"
#include "Combine.hpp"
#include "CubeMap.hpp"
#include "DescriptorPool.hpp"
#include "Fbo.hpp"
#include "Filter.hpp"
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

        VkCommandBuffer primaryCmdBuffer;     // Duplicated
        VkCommandBuffer primaryCmdBufferCube; // Duplicated
        VkExtent2D      swapChainExtent;      // Stays

        DescriptorPool descriptorPool; // Duplicated
        VkCommandPool  commandPool;    // Duplicated

        Fbo      renderedImageFbo;
        Fbo      temporaryCubeMapFbo;
        VkFormat format;
        Memory   memory;

        GraphicsPipeline particlePipeline;
        GraphicsPipeline cubeMapPipeline;

        std::vector<Fbo *>              fbos;
        std::vector<Filter *>           filters;
        std::vector<Combine *>          combines;
        std::vector<GraphicsPipeline *> pipelines; // Goes
        std::vector<Model *>            models;    // Goes
        std::vector<Camera *>           cameras;   // Goes
        std::vector<Light *>            lights;
        std::vector<ParticleSystem *>   particleSystems;

        VkFence inFlightFence;
        VkFence inFlightFenceCube;

        CubeMap skyboxCubemap;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // Goes
        VkSampleCountFlagBits maxMsaaSamples;
        int32_t               targetMsaaSamples = 1;

    public:
        void CreateScene(VkPhysicalDevice & physicalDevice,
                         VkDevice &         device,
                         VkQueue &          graphicsQueue,
                         VkFormat &         imageFormant,
                         VkExtent2D         swapChainExtent);

        void DestroyScene();

        void RebuildSwapResources(VkExtent2D newExtent);

    private:
        void CreateCommandPool();
        void CreateCommandBuffer(); // Missing implementation
        void CreateSyncObjects();

        void RebuildCommandBuffer();     // Missing implementation
        void RebuildCubeCommandBuffer(); // Missing implementation

    private:
        void DestroyCommandPool();
        void DestroyCommandBuffer(); // Missing implementation
        void DestroySyncObjects();

    private:
        void DestroySwapResources();

    private:
        void RenderCubeMap(Entity & transparentEntity);

    private:
        VkSampleCountFlagBits GetTargetSampleCount(int32_t targetSampleCount);

    public:
        void Render(Camera & camera, float deltaTime);
        void SetMSAA(int32_t targetMsaaSamples);

        Fbo & GetRenderedImage();

    public:
        GraphicsPipeline & CreatePipeline(const char * complexVertexFile,
                                          const char * complexFragmentFile,
                                          const char * simpleVertexFile,
                                          const char * simpleFragmentFile);

        Model & CreateModel(const char *       modelFile,
                            const char *       colorFile,
                            const char *       normalFile,
                            const char *       specularFile,
                            GraphicsPipeline & pipeline);

        Camera &         CreateCamera(float fov, float near, float far);
        Entity &         CreateEntity(Model & model, bool isReflectRefractive, bool isAnimated);
        Light &          CreateLight(bool isSpotlight);
        Fbo &            CreateFbo();
        Filter &         CreateFilter(const char * fragmentFile);
        Combine &        CreateCombine(const char * fragmentFile);
        ParticleSystem & CreateParticleSystem(uint32_t     numStages,
                                              const char * texture,
                                              uint32_t     hStages,
                                              uint32_t     vStages,
                                              uint32_t     totalStages);

        VkSampleCountFlagBits GetMsaaSamples();
    };
} // namespace sckz