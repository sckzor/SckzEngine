#include "Vulkan.hpp"

namespace sckz
{
    VkResult Vulkan::CreateDebugUtilsMessengerEXT(VkInstance                                 instance,
                                                  const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo,
                                                  const VkAllocationCallbacks *              pAllocator,
                                                  VkDebugUtilsMessengerEXT *                 pDebugMessenger)
    {
        auto func
            = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void Vulkan::DestroyDebugUtilsMessengerEXT(VkInstance                    instance,
                                               VkDebugUtilsMessengerEXT      debugMessenger,
                                               const VkAllocationCallbacks * pAllocator)
    {
        auto func
            = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                                                         VkDebugUtilsMessageTypeFlagsEXT              messageType,
                                                         const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                         void *                                       pUserData)
    {
        std::cerr << "[Validation Layer] " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    std::vector<const char *> Vulkan::GetRequiredExtensions()
    {
        uint32_t      glfwExtensionCount = 0;
        const char ** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    VkDebugUtilsMessengerCreateInfoEXT Vulkan::MakeDebugMessengerCreateInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo {};
        createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        return createInfo;
    }

    uint32_t Vulkan::GetMaximumSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts
                                  & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return 64;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return 32;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return 16;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return 8;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return 4;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return 2;
        }

        return 1;
    }

    bool Vulkan::CheckValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char * layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto & layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    bool Vulkan::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto & extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    Vulkan::SwapChainSupportDetails Vulkan::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    bool Vulkan::IsDeviceSuitable(VkPhysicalDevice device)
    {
        HelperMethods::QueueFamilyIndices indices = HelperMethods::FindQueueFamilies(device, surface);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    VkSurfaceFormatKHR Vulkan::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats)
    {
        for (const auto & availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR Vulkan::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> & availablePresentModes)
    {
        for (const auto & availablePresentMode : availablePresentModes)
        {
            if (fps == 0)
            {
                if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
                {
                    return availablePresentMode;
                }
            }
            else
            {
                if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    return availablePresentMode;
                }
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Vulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window->GetWindow(), &width, &height);

            VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

            actualExtent.width  = std::max(capabilities.minImageExtent.width,
                                          std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height,
                                           std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    void Vulkan::CreateInstance()
    {
        if (enableValidationLayers && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "No Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions                    = GetRequiredExtensions();
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = MakeDebugMessengerCreateInfo();
            createInfo.pNext                                   = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void Vulkan::SetupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = MakeDebugMessengerCreateInfo();

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void Vulkan::CreateSurface()
    {
        if (glfwCreateWindowSurface(instance, window->GetWindow(), nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void Vulkan::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        uint32_t highestScore = -1;
        for (const auto & device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void Vulkan::CreateLogicalDevice()
    {
        HelperMethods::QueueFamilyIndices indices = HelperMethods::FindQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo {};
            queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount       = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos    = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    void Vulkan::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR   presentMode   = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D         extent        = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        HelperMethods::QueueFamilyIndices indices = HelperMethods::FindQueueFamilies(physicalDevice, surface);
        uint32_t queueFamilyIndices[]             = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = presentMode;
        createInfo.clipped        = VK_TRUE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        std::vector<VkImage> rawSwapChainImages;
        swapChainImages.resize(imageCount);
        rawSwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, rawSwapChainImages.data());

        for (int i = 0; i < imageCount; i++)
        {
            swapChainImages[i].CreateImage(device, rawSwapChainImages[i], surfaceFormat.format, 1);
        }

        swapChainExtent = extent;
    }

    void Vulkan::CreateImageViews()
    {
        for (uint32_t i = 0; i < swapChainImages.size(); i++)
        {
            swapChainImages[i].CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void Vulkan::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment {};
        colorAttachment.format = swapChainImages[0].GetFormat();

        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;

        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAttachment {};
        depthAttachment.format         = HelperMethods::FindDepthFormat(physicalDevice);
        depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve {};
        colorAttachmentResolve.format         = swapChainImages[0].GetFormat();
        colorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef {};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask
            = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask
            = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;

        renderPassInfo.pSubpasses = &subpass;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void Vulkan::CreateCommandPool()
    {
        HelperMethods::QueueFamilyIndices queueFamilyIndices
            = HelperMethods::FindQueueFamilies(physicalDevice, surface);

        VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }

    void Vulkan::CreateColorResources()
    {
        VkFormat colorFormat = swapChainImages[0].GetFormat();

        colorImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               VK_SAMPLE_COUNT_1_BIT,
                               colorFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               device,
                               physicalDevice,
                               memory,
                               graphicsQueue);
        colorImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Vulkan::CreateDepthResources()
    {
        VkFormat depthFormat = HelperMethods::FindDepthFormat(physicalDevice);

        depthImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               VK_SAMPLE_COUNT_1_BIT,
                               depthFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               device,
                               physicalDevice,
                               memory,
                               graphicsQueue);
        depthImage.CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void Vulkan::CreateFramebuffers()
    {
        swapChainFramebuffers.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++)
        {
            std::vector<VkImageView> attachments;

            attachments.push_back(swapChainImages[i].GetImageView());
            attachments.push_back(depthImage.GetImageView());

            VkFramebufferCreateInfo framebufferInfo {};
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      = renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments    = attachments.data();
            framebufferInfo.width           = swapChainExtent.width;
            framebufferInfo.height          = swapChainExtent.height;
            framebufferInfo.layers          = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void Vulkan::CreateSyncObjects()
    {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
                || vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
                || vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a "
                                         "frame!");
            }
        }
    }

    void Vulkan::DestroyInstance() { vkDestroyInstance(instance, nullptr); }

    void Vulkan::DestroyDebugMessenger()
    {
        if (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
    }

    void Vulkan::DestroySurface() { vkDestroySurfaceKHR(instance, surface, nullptr); }

    void Vulkan::DestroyLogicalDevice() { vkDestroyDevice(device, nullptr); }

    void Vulkan::DestroySwapChain() { vkDestroySwapchainKHR(device, swapChain, nullptr); }

    void Vulkan::DestroyImageViews()
    {
        for (int i = 0; i < swapChainImages.size(); i++)
        {
            swapChainImages[i].DestroyImage();
        }
    }

    void Vulkan::DestroyRenderPass() { vkDestroyRenderPass(device, renderPass, nullptr); }

    void Vulkan::DestroyCommandPool() { vkDestroyCommandPool(device, commandPool, nullptr); }

    void Vulkan::DestroyFramebuffers()
    {
        for (auto framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
    }

    void Vulkan::DestroySyncObjects()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
    }

    void Vulkan::CreateVulkan(Window & window)
    {
        this->window = &window;
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        memory.CreateMemory(device, physicalDevice, 0xFFFFFFF);
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateCommandPool();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffers();
        descriptorPool.CreateDescriptorPool(device, swapChainImages.size());
        CreateSyncObjects();
    }

    void Vulkan::DestroyVulkan()
    {
        DestroySwapResources();
        DestroySyncObjects();
        DestroyCommandPool();
        memory.FreeMemory();
        memory.DestroyMemory();
        DestroyLogicalDevice();
        DestroyDebugMessenger();
        DestroySurface();
        DestroyInstance();
    }

    void Vulkan::RebuildSwapChain()
    {
        DestroySwapResources();

        CreateSwapChain();

        for (uint32_t i = 0; i < scenes.size(); i++)
        {
            scenes[i]->RebuildSwapResources(swapChainExtent);
        }

        CreateImageViews();
        CreateRenderPass();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffers();
        descriptorPool.CreateDescriptorPool(device, swapChainImages.size());
        for (uint32_t i = 0; i < fboPipelines.size(); i++)
        {
            fboPipelines[i]->CreatePipeline(device, swapChainExtent, renderPass, VK_SAMPLE_COUNT_1_BIT, true);
        }
    }

    void Vulkan::DestroySwapResources()
    {
        vkDeviceWaitIdle(device);
        vkQueueWaitIdle(graphicsQueue);

        depthImage.DestroyImage();
        colorImage.DestroyImage();
        DestroyFramebuffers();
        DestroyRenderPass();
        DestroyImageViews();
        DestroySwapChain();
        descriptorPool.DestroyDescriptorPool();
        for (uint32_t i = 0; i < fboPipelines.size(); i++)
        {
            fboPipelines[i]->DestroyPipeline();
        }
    }

    void Vulkan::CreateCommandBuffers(Scene * scene, GraphicsPipeline * pipeline)
    {
        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass        = renderPass;
            renderPassInfo.framebuffer       = (swapChainFramebuffers)[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapChainExtent;

            std::array<VkClearValue, 2> clearValues {};
            clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues    = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

            VkDescriptorSet ds;

            pipeline->BindImage(scene->GetRenderedImage(), descriptorPool, &ds);

            vkCmdBindDescriptorSets(commandBuffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline->GetPieplineLayout(),
                                    0,
                                    1,
                                    &ds,
                                    0,
                                    nullptr);

            vkCmdDraw(commandBuffers[i],
                      3,
                      1,
                      0,
                      0); // Uses the wacky shader from Sascha to draw the image to the screen

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void Vulkan::Present(Scene & scene, GraphicsPipeline & pipeline)
    {
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        if (lastRenderedScene != &scene || lastRenderedPipeline != &pipeline || scene.IsUpdated())
        {
            CreateCommandBuffers(&scene, &pipeline);

            lastRenderedPipeline = &pipeline;
            lastRenderedScene    = &scene;
        }

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device,
                                                swapChain,
                                                UINT64_MAX,
                                                imageAvailableSemaphores[currentFrame],
                                                VK_NULL_HANDLE,
                                                &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RebuildSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore          waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[]     = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount         = 1;
        submitInfo.pWaitSemaphores            = waitSemaphores;
        submitInfo.pWaitDstStageMask          = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[]  = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount  = 1;
        presentInfo.pSwapchains     = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            framebufferResized = false;
            RebuildSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        // TODO: Probably fix this because although it works it is ugly as h*ll
        if (fps > 0)
        {
            point1                                              = std::chrono::system_clock::now();
            std::chrono::duration<double, std::milli> work_time = point1 - point2;

            if (work_time.count() < (1.0 / fps) * 1000.0)
            {
                std::chrono::duration<double, std::milli> delta_ms((1.0 / fps) * 1000.0 - work_time.count());
                auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
                std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
            }

            point2                                               = std::chrono::system_clock::now();
            std::chrono::duration<double, std::milli> sleep_time = point1 - point2;
        }

        lastTime    = currentTime;
        currentTime = std::chrono::high_resolution_clock::now();
        deltaTime   = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    }

    void Vulkan::Update() { }

    float Vulkan::GetDeltaT() { return deltaTime; }

    void Vulkan::SetFPS(int32_t fps)
    {
        this->fps = fps;
        RebuildSwapChain();
    }

    Scene & Vulkan::CreateScene()
    {
        scenes.push_back(new Scene());
        VkFormat colorFormat = swapChainImages[0].GetFormat();
        scenes.back()->CreateScene(physicalDevice,
                                   device,
                                   graphicsQueue,
                                   colorFormat,
                                   swapChainExtent); // poggers moment lmao!
        return *scenes.back();
    }

    GraphicsPipeline & Vulkan::CreateFBOPipeline(const char * fragShader)
    {
        fboPipelines.push_back(new GraphicsPipeline());
        fboPipelines.back()->CreatePipeline(device,
                                            swapChainExtent,
                                            renderPass,
                                            VK_SAMPLE_COUNT_1_BIT,
                                            "Resources/fbo_vertex.spv",
                                            fragShader,
                                            true);

        return *fboPipelines.back();
    }
} // namespace sckz