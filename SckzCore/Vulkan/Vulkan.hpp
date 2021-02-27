#pragma once
#include "../Include.hpp"
#include "../Window/Window.hpp"
#include "CommandBuffer.hpp"
#include "DescriptorPool.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Model.hpp"

namespace sckz
{
    class Image;
    class Vulkan
    {
    private: // Private Structures
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
        };

        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

    private: // Private Members
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        static const int MAX_FRAMES_IN_FLIGHT = 2;

        Window * window;

        VkInstance               instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR             surface;
        VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE;
        VkSampleCountFlagBits    msaaSamples    = VK_SAMPLE_COUNT_1_BIT;
        VkDevice                 device;
        VkQueue                  graphicsQueue;
        VkQueue                  presentQueue;

        Memory memory;

        VkSwapchainKHR             swapChain;
        std::vector<Image>         swapChainImages;
        VkExtent2D                 swapChainExtent;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        std::vector<VkCommandBuffer> primaryCmdBuffers;

        VkRenderPass renderPass;

        DescriptorPool descriptorPool;
        VkCommandPool  commandPool;

        Image colorImage;
        Image depthImage;

        std::vector<GraphicsPipeline *> pipelines;
        std::vector<Model *>            models;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence>     inFlightFences;
        std::vector<VkFence>     imagesInFlight;

        const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        size_t                          currentFrame     = 0;

        bool framebufferResized = false;

    public: // Public static functions
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT        messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                            void *                                       pUserData);

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance                                 instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
                                                     const VkAllocationCallbacks *              pAllocator,
                                                     VkDebugUtilsMessengerEXT *                 pDebugMessenger);

        static void DestroyDebugUtilsMessengerEXT(VkInstance                    instance,
                                                  VkDebugUtilsMessengerEXT      debugMessenger,
                                                  const VkAllocationCallbacks * pAllocator);

    private: // Private helper functions
        std::vector<const char *>          GetRequiredExtensions();
        VkDebugUtilsMessengerCreateInfoEXT MakeDebugMessengerCreateInfo();
        VkSampleCountFlagBits              GetMaxUsableSampleCount();
        bool                               CheckValidationLayerSupport();
        QueueFamilyIndices                 FindQueueFamilies(VkPhysicalDevice device);
        bool                               CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails            QuerySwapChainSupport(VkPhysicalDevice device);
        bool                               IsDeviceSuitable(VkPhysicalDevice device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);
        VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);
        VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);
        VkFormat           FindDepthFormat();
        VkFormat           FindSupportedFormat(const std::vector<VkFormat> & candidates,
                                               VkImageTiling                 tiling,
                                               VkFormatFeatureFlags          features);

    private: // Private creation functions
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateRenderPass();
        void CreateCommandPool();
        void CreatePrimaryCmdBuffers();
        void CreateColorResources();
        void CreateDepthResources();
        void CreateFramebuffers();
        void CreateSyncObjects();

    private: // Swapchain recreation functions
        void RebuildSwapChain();
        void DestroySwapResources();

    private: // Private destruction functions
        void DestroyInstance();
        void DestroyDebugMessenger();
        void DestroySurface();
        void DestroyLogicalDevice();
        void DestroySwapChain();
        void DestroyImageViews();
        void DestroyRenderPass();
        void DestroyCommandPool();
        void DestroyFramebuffers();
        void DestroySyncObjects();

    public: // Public member functions
        void CreateVulkan(Window & window);
        void DestroyVulkan();
        void Update();

    public:
        GraphicsPipeline & CreatePipeline(const char * vertexFile, const char * fragmentFile);

        Model & CreateModel(const char * modelFile, const char * textureFile, GraphicsPipeline & pipeline);
    };
} // namespace sckz