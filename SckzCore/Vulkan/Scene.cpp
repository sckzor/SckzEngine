#include "Scene.hpp"

namespace sckz
{

    void Scene::CreateScene(VkPhysicalDevice & physicalDevice,
                            VkDevice &         device,
                            VkQueue &          graphicsQueue,
                            VkFormat &         imageFormat,
                            VkExtent2D         swapChainExtent)
    {
        this->physicalDevice  = &physicalDevice;
        this->device          = &device;
        this->graphicsQueue   = &graphicsQueue;
        this->swapChainExtent = swapChainExtent;
        this->format          = imageFormat;

        memory.CreateMemory(device, physicalDevice, 0xFFFFFFF);

        CreateImage();
        CreateRenderPass();
        CreateCommandPool();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffers();
        descriptorPool.CreateDescriptorPool(device, 1);
        CreateSyncObjects();
    }

    void Scene::DestroyScene()
    {
        DestroySwapResources();

        for (int i = 0; i < models.size(); i++)
        {
            models[i]->DestroyModel();
            delete models[i];
        }

        for (int i = 0; i < cameras.size(); i++)
        {
            cameras[i]->DestroyCamera();
            delete cameras[i];
        }

        DestroySyncObjects();
        DestroyCommandPool();
        memory.DestroyMemory();

        for (int i = 0; i < pipelines.size(); i++)
        {
            delete pipelines[i];
        }
        for (int i = 0; i < lights.size(); i++)
        {
            lights[i]->DestroyLight();
            delete lights[i];
        }
    }

    void Scene::RebuildSwapResources(VkExtent2D newExtent)
    {
        swapChainExtent = newExtent;
        isUpdated       = true;
        DestroySwapResources();

        CreateImage();
        CreateRenderPass();
        for (int i = 0; i < pipelines.size(); i++)
        {
            pipelines[i]->CreatePipeline(*device, swapChainExtent, renderPass, msaaSamples, false);
        }
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffers();

        descriptorPool.CreateDescriptorPool(*device, 1);
        for (int i = 0; i < models.size(); i++)
        {
            models[i]->RebuildSwapResources(descriptorPool, swapChainExtent);
        }
        CreateCommandBuffer();

        for (int i = 0; i < cameras.size(); i++)
        {
            cameras[i]->UpdateExtent(swapChainExtent);
        }
    }

    void Scene::DestroySwapResources()
    {
        vkQueueWaitIdle(*graphicsQueue);

        DestroyFramebuffers();
        for (int i = 0; i < pipelines.size(); i++)
        {
            pipelines[i]->DestroyPipeline();
        }
        DestroyRenderPass();
        descriptorPool.DestroyDescriptorPool();
        depthImage.DestroyImage();
        colorImage.DestroyImage();

        renderedImage.DestroyImage();
    }

    void Scene::DestroyFramebuffers() { vkDestroyFramebuffer(*device, renderedImageFrameBuffer, nullptr); }
    void Scene::DestroyRenderPass() { vkDestroyRenderPass(*device, renderPass, nullptr); }
    void Scene::DestroySyncObjects() { vkDestroyFence(*device, inFlightFence, nullptr); }
    void Scene::DestroyCommandPool() { vkDestroyCommandPool(*device, commandPool, nullptr); }

    void Scene::CreateImage()
    {
        renderedImage.CreateImage(swapChainExtent.width,
                                  swapChainExtent.height,
                                  1,
                                  VK_SAMPLE_COUNT_1_BIT,
                                  this->format,
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  *this->device,
                                  *this->physicalDevice,
                                  memory,
                                  *this->graphicsQueue);
        renderedImage.CreateTextureSampler();
        renderedImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Scene::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment {};
        colorAttachment.format         = renderedImage.GetFormat();
        colorAttachment.samples        = msaaSamples;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        if (msaaSamples == VK_SAMPLE_COUNT_1_BIT)
        {
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        else
        {
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        VkAttachmentDescription depthAttachment {};
        depthAttachment.format         = HelperMethods::FindDepthFormat(*physicalDevice);
        depthAttachment.samples        = msaaSamples;
        depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve {};
        colorAttachmentResolve.format         = renderedImage.GetFormat();
        colorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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
        if (msaaSamples != VK_SAMPLE_COUNT_1_BIT)
        {
            subpass.pResolveAttachments = &colorAttachmentResolveRef;
        }

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

        if (msaaSamples != VK_SAMPLE_COUNT_1_BIT)
        {
            attachments.push_back(colorAttachmentResolve);
        }

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;

        renderPassInfo.pSubpasses = &subpass;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        if (vkCreateRenderPass(*device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void Scene::CreateCommandPool()
    {
        HelperMethods::QueueFamilyIndices queueFamilyIndices = HelperMethods::FindQueueFamilies(*physicalDevice);

        VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(*device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }

    void Scene::CreateColorResources()
    {
        VkFormat colorFormat = renderedImage.GetFormat();

        colorImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               msaaSamples,
                               colorFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               *device,
                               *physicalDevice,
                               memory,
                               *graphicsQueue);
        colorImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Scene::CreateDepthResources()
    {
        VkFormat depthFormat = HelperMethods::FindDepthFormat(*physicalDevice);

        depthImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               msaaSamples,
                               depthFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               *device,
                               *physicalDevice,
                               memory,
                               *graphicsQueue);
        depthImage.CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void Scene::CreateFramebuffers()
    {
        std::vector<VkImageView> attachments;

        if (msaaSamples == VK_SAMPLE_COUNT_1_BIT)
        {
            attachments.push_back(renderedImage.GetImageView());
            attachments.push_back(depthImage.GetImageView());
        }
        else
        {
            attachments.push_back(colorImage.GetImageView());
            attachments.push_back(depthImage.GetImageView());
            attachments.push_back(renderedImage.GetImageView());
        }

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = swapChainExtent.width;
        framebufferInfo.height          = swapChainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &renderedImageFrameBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    void Scene::CreateSyncObjects()
    {
        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(*device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    void Scene::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &primaryCmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(primaryCmdBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = renderPass;
        renderPassInfo.framebuffer       = renderedImageFrameBuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBeginRenderPass(primaryCmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
        std::vector<VkCommandBuffer> buffers;
        for (size_t j = 0; j < models.size(); j++)
        {
            buffers.push_back((models[j]->GetCommandBuffer()));
        }

        vkCmdExecuteCommands(primaryCmdBuffer, buffers.size(), buffers.data());

        vkCmdEndRenderPass(primaryCmdBuffer);

        if (vkEndCommandBuffer(primaryCmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    VkSampleCountFlagBits Scene::GetTargetSampleCount(int32_t targetSampleCount) // TODO: make this mutable
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(*physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts
                                  & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

        VkSampleCountFlagBits targetSampleBits;

        if (targetSampleCount == -1)
        {
            targetSampleCount = maxMsaaSamples;
        }

        switch (targetSampleCount)
        {
            case VK_SAMPLE_COUNT_64_BIT:
                targetSampleBits = VK_SAMPLE_COUNT_64_BIT;
                break;

            case VK_SAMPLE_COUNT_32_BIT:
                targetSampleBits = VK_SAMPLE_COUNT_32_BIT;
                break;
            case VK_SAMPLE_COUNT_16_BIT:
                targetSampleBits = VK_SAMPLE_COUNT_16_BIT;
                break;

            case VK_SAMPLE_COUNT_8_BIT:
                targetSampleBits = VK_SAMPLE_COUNT_8_BIT;
                break;

            case VK_SAMPLE_COUNT_4_BIT:
                targetSampleBits = VK_SAMPLE_COUNT_4_BIT;
                break;

            case VK_SAMPLE_COUNT_2_BIT:
                targetSampleBits = VK_SAMPLE_COUNT_2_BIT;
                break;

            default:
                targetSampleBits = VK_SAMPLE_COUNT_1_BIT;
        }

        if (targetSampleBits > counts)
        {
            // Max MSAA on this system is 8

            std::cout << "[WARN] MSAA sample count too large for this system, falling back to a sample size of 1!"
                      << std::endl;

            return VK_SAMPLE_COUNT_1_BIT;
        }

        return targetSampleBits;
    }

    Entity & Scene::CreateEntity(Model & model)
    {
        Entity & entity = model.CreateEntity();
        CreateCommandBuffer();
        entity.LoadLights(lights);
        return entity;
    }

    Light & Scene::CreateLight()
    {
        lights.push_back(new Light());
        return *lights.back();
    }

    GraphicsPipeline & Scene::CreatePipeline(const char * vertexFile, const char * fragmentFile)
    {
        pipelines.push_back(new GraphicsPipeline());
        pipelines.back()
            ->CreatePipeline(*device, swapChainExtent, renderPass, msaaSamples, vertexFile, fragmentFile, false);
        return *pipelines.back();
    }

    Model & Scene::CreateModel(const char * modelFile, const char * textureFile, GraphicsPipeline & pipeline)
    {
        models.push_back(new Model());
        models.back()->CreateModel(textureFile,
                                   modelFile,
                                   commandPool,
                                   renderPass,
                                   *device,
                                   *physicalDevice,
                                   renderedImageFrameBuffer,
                                   &pipeline,
                                   descriptorPool,
                                   swapChainExtent,
                                   memory,
                                   *graphicsQueue);
        return *models.back();
    }

    Camera & Scene::CreateCamera(float fov, float near, float far)
    {
        cameras.push_back(new Camera());
        cameras.back()->CreateCamera(fov, near, far, swapChainExtent);
        return *cameras.back();
    }

    void Scene::SetMSAA(int32_t targetMsaaSamples)
    {
        this->msaaSamples = GetTargetSampleCount(targetMsaaSamples);
        RebuildSwapResources(swapChainExtent);
    }

    void Scene::Render(Camera & camera)
    {
        vkWaitForFences(*device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(*device, 1, &inFlightFence);

        for (int i = 0; i < models.size(); i++)
        {
            models[i]->Update(camera);
        }

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount     = 0;
        submitInfo.pWaitDstStageMask      = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &primaryCmdBuffer;

        submitInfo.signalSemaphoreCount = 0;

        if (vkQueueSubmit(*graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer! (scene)");
        }

        vkQueueWaitIdle(*graphicsQueue);
    }

    bool Scene::IsUpdated() { return isUpdated; }

    Image & Scene::GetRenderedImage()
    {
        return renderedImage;
        isUpdated = false;
    }

} // namespace sckz