#pragma once
#include "../../Include.hpp"
#include "../Window/Window.hpp"
#include "Camera.hpp"
#include "CommandBuffer.hpp"
#include "DescriptorPool.hpp"
#include "Fbo.hpp"
#include "GraphicsPipeline.hpp"
#include "Gui.hpp"
#include "HelperMethods.hpp"
#include "Image.hpp"
#include "Model.hpp"
#include "Scene.hpp"

namespace sckz
{
    class Image;
    class Vulkan
    {
    private: // Private Structures
        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR        capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR>   presentModes;
        };

    private: // Private Members
#ifdef SCKZ_DEBUG
        const bool enableValidationLayers = true;
#else
        const bool enableValidationLayers = false;
#endif

        static const int MAX_FRAMES_IN_FLIGHT = 2; // Stays

        Window * window; // Stays

        VkInstance               instance;                        // Stays
        VkDebugUtilsMessengerEXT debugMessenger;                  // Stays
        VkSurfaceKHR             surface;                         // Stays
        VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE; // Stays
        VkDevice                 device;                          // Stays
        VkQueue                  graphicsQueue;                   // Stays
        VkQueue                  presentQueue;                    // Stays

        Memory memory;

        VkSwapchainKHR             swapChain;             // Stays
        std::vector<Image>         swapChainImages;       // Stays
        VkExtent2D                 swapChainExtent;       // Stays
        std::vector<VkFramebuffer> swapChainFramebuffers; // Stays

        std::vector<VkCommandBuffer> commandBuffers; // Duplicated
        std::vector<VkCommandBuffer> primaryCommandBuffers;

        VkRenderPass renderPass; // Duplicated

        DescriptorPool descriptorPool; // Duplicated
        VkCommandPool  commandPool;    // Duplicated

        Image colorImage; // Duplicated
        Image depthImage; // Duplicated

        std::vector<VkSemaphore> imageAvailableSemaphores; // Duplicated
        std::vector<VkSemaphore> renderFinishedSemaphores; // Duplicated
        std::vector<VkFence>     inFlightFences;           // Duplicated
        std::vector<VkFence>     imagesInFlight;           // Duplicated

        std::vector<GraphicsPipeline *> fboPipelines;
        GraphicsPipeline                guiPipeline;
        std::vector<Scene *>            scenes;
        std::vector<Gui *>              guis;

        const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" }; // Stays
        const std::vector<const char *> deviceExtensions
            = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MULTIVIEW_EXTENSION_NAME }; // Stays

        size_t currentFrame = 0; // Stays

        std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;    // Stays
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTime; // Stays
        float                                                       deltaTime;   // Stays

        std::chrono::system_clock::time_point point1
            = std::chrono::system_clock::now(); // Used to add delay to the loop accurately for limiting FPS, we cant
                                                // use deltaTime because it will keep increaseing
                                                // Stays
        std::chrono::system_clock::time_point point2 = std::chrono::system_clock::now(); // Stays

        bool framebufferResized = false; // Stays

        int32_t fps = 0; // Stays

        Fbo *              lastRenderedFbo;
        GraphicsPipeline * lastRenderedPipeline;

        Fbo dummyFbo;

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
        VkSampleCountFlagBits              GetTargetSampleCount(int32_t targetSampleCountt);
        bool                               CheckValidationLayerSupport();
        bool                               CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails            QuerySwapChainSupport(VkPhysicalDevice device);
        bool                               IsDeviceSuitable(VkPhysicalDevice device);

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);
        VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes);
        VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);

    private: // Private creation functions
        // Stays here
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();

        // Duplicated in the scene class

        void CreateImageViews();
        void CreateRenderPass();
        void CreateCommandPool();
        void CreateCommandBuffers(Fbo * fbo, GraphicsPipeline * pipeline);
        void CreateColorResources();
        void CreateDepthResources();
        void CreateFramebuffers();
        void CreateSyncObjects();

        void RebuildCommandBuffers(Scene * scene, GraphicsPipeline * pipeline);

    private: // Swapchain recreation functions
        void RebuildSwapChain();
        void DestroySwapResources();

    private: // Private destruction functions
        // Stays here
        void DestroyInstance();
        void DestroyDebugMessenger();
        void DestroySurface();
        void DestroyLogicalDevice();
        void DestroySwapChain();

        // Goes to the scene class

        void DestroyImageViews();
        void DestroyRenderPass();
        void DestroyCommandPool();
        void DestroyFramebuffers();
        void DestroySyncObjects();

    public: // Public member functions
        // Stays here
        void CreateVulkan(Window & window);
        void DestroyVulkan();

        void SetFPS(int32_t fps);

        uint32_t GetMaximumSampleCount();
        float    GetDeltaT();

        void Present(Fbo & fbo, GraphicsPipeline & pipeline);

    public:
        Scene & CreateScene();

        Gui & CreateGUI(const char * texture);

        GraphicsPipeline & CreateFBOPipeline(const char * fragShader);
    };
} // namespace sckz