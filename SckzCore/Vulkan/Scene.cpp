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

        CreateCommandPool();

        descriptorPool.CreateDescriptorPool(device, 1);
        CreateSyncObjects();

        renderedImageFbo.CreateFBO(physicalDevice,
                                   device,
                                   memory,
                                   graphicsQueue,
                                   format,
                                   msaaSamples,
                                   swapChainExtent,
                                   commandPool,
                                   false);

        temporaryCubeMapFbo.CreateFBO(physicalDevice,
                                      device,
                                      memory,
                                      graphicsQueue,
                                      format,
                                      msaaSamples,
                                      swapChainExtent,
                                      commandPool,
                                      true);

        particlePipeline.CreatePipeline(*this->device,
                                        renderedImageFbo,
                                        "Resources/particle_vertex.spv",
                                        "Resources/particle_fragment.spv",
                                        temporaryCubeMapFbo,
                                        nullptr,
                                        nullptr,
                                        GraphicsPipeline::PipelineType::PARTICLE_PIPELINE);

        cubeMapPipeline.CreatePipeline(
            *this->device,
            renderedImageFbo,
            "Resources/skybox_vertex.spv",
            "Resources/skybox_fragment.spv",
            temporaryCubeMapFbo,
            "Resources/cubemap_render_cube_vertex.spv", // THIS MUST CHANGE TO MAKE IT SO THAT THERE IS
                                                        // ANOTHER SHADER WITH MULTIPLE VIEW MATRICEIS.
            "Resources/cubemap_render_cube_fragment.spv",
            GraphicsPipeline::PipelineType::CUBEMAP_PIPELINE);

        skyboxCubemap.CreateCubeMap("Resources/posz.jpg",
                                    "Resources/negz.jpg",
                                    "Resources/posx.jpg",
                                    "Resources/negx.jpg",
                                    "Resources/posy.jpg",
                                    "Resources/negy.jpg",
                                    device,
                                    physicalDevice,
                                    memory,
                                    commandPool,
                                    graphicsQueue,
                                    cubeMapPipeline,
                                    descriptorPool,
                                    50);
    }

    void Scene::DestroyScene()
    {
        DestroySwapResources();

        skyboxCubemap.DestroyCubeMap();
        renderedImageFbo.DestroyFBO();
        temporaryCubeMapFbo.DestroyFBO();

        for (uint32_t i = 0; i < models.size(); i++)
        {
            models[i]->DestroyModel();
            delete models[i];
        }

        for (uint32_t i = 0; i < particleSystems.size(); i++)
        {
            particleSystems[i]->DestroyParticleSystem();
            delete particleSystems[i];
        }

        for (uint32_t i = 0; i < cameras.size(); i++)
        {
            cameras[i]->DestroyCamera();
            delete cameras[i];
        }

        for (uint32_t i = 0; i < fbos.size(); i++)
        {
            fbos[i]->DestroyFBO();
            delete fbos[i];
        }

        for (uint32_t i = 0; i < filters.size(); i++)
        {
            filters[i]->DestroyFilter();
            delete filters[i];
        }

        for (uint32_t i = 0; i < combines.size(); i++)
        {
            combines[i]->DestroyCombine();
            delete combines[i];
        }

        DestroySyncObjects();
        DestroyCommandPool();
        memory.DestroyMemory();

        for (uint32_t i = 0; i < pipelines.size(); i++)
        {
            delete pipelines[i];
        }
        for (uint32_t i = 0; i < lights.size(); i++)
        {
            lights[i]->DestroyLight();
            delete lights[i];
        }
    }

    void Scene::RebuildSwapResources(VkExtent2D newExtent)
    {
        swapChainExtent = newExtent;
        DestroySwapResources();

        renderedImageFbo.RebuildSwapResources(msaaSamples, swapChainExtent);
        temporaryCubeMapFbo.RebuildSwapResources(msaaSamples, swapChainExtent);

        particlePipeline.CreatePipeline(*device, renderedImageFbo, temporaryCubeMapFbo);
        cubeMapPipeline.CreatePipeline(*device, renderedImageFbo, temporaryCubeMapFbo);

        for (uint32_t i = 0; i < pipelines.size(); i++)
        {
            pipelines[i]->CreatePipeline(*device, renderedImageFbo, temporaryCubeMapFbo);
        }

        descriptorPool.CreateDescriptorPool(*device, 1);

        skyboxCubemap.RebuildSwapResources(descriptorPool);
        for (uint32_t i = 0; i < models.size(); i++)
        {
            models[i]->RebuildSwapResources(descriptorPool, swapChainExtent);
        }

        for (uint32_t i = 0; i < fbos.size(); i++)
        {
            fbos[i]->RebuildSwapResources(msaaSamples, swapChainExtent);
        }

        for (uint32_t i = 0; i < particleSystems.size(); i++)
        {
            particleSystems[i]->RebuildSwapResources(descriptorPool, swapChainExtent);
        }
        CreateCommandBuffer();

        for (uint32_t i = 0; i < cameras.size(); i++)
        {
            cameras[i]->UpdateExtent(swapChainExtent);
        }

        for (uint32_t i = 0; i < filters.size(); i++)
        {
            filters[i]->RebuildSwapResources(descriptorPool, msaaSamples, swapChainExtent);
        }

        for (uint32_t i = 0; i < combines.size(); i++)
        {
            combines[i]->RebuildSwapResources(descriptorPool, msaaSamples, swapChainExtent);
        }
    }

    void Scene::DestroySwapResources()
    {
        vkQueueWaitIdle(*graphicsQueue);

        for (uint32_t i = 0; i < pipelines.size(); i++)
        {
            pipelines[i]->DestroyPipeline();
        }

        particlePipeline.DestroyPipeline();
        cubeMapPipeline.DestroyPipeline();
        descriptorPool.DestroyDescriptorPool();
    }

    void Scene::DestroySyncObjects()
    {
        vkDestroyFence(*device, inFlightFence, nullptr);
        vkDestroyFence(*device, inFlightFenceCube, nullptr);
    }

    void Scene::DestroyCommandPool() { vkDestroyCommandPool(*device, commandPool, nullptr); }

    void Scene::CreateCommandPool()
    {
        HelperMethods::QueueFamilyIndices queueFamilyIndices = HelperMethods::FindQueueFamilies(*physicalDevice);

        VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(*device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }

    void Scene::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(*device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }

        if (vkCreateFence(*device, &fenceInfo, nullptr, &inFlightFenceCube) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    void Scene::RebuildCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(primaryCmdBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // std::cout << "Primary Command Buffer: " << primaryCmdBuffer << std::endl;

        VkRenderPassBeginInfo renderPassInfo {};
        renderedImageFbo.GetRenderPassBeginInfo(&renderPassInfo);

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

        for (size_t j = 0; j < particleSystems.size(); j++)
        {
            buffers.push_back((particleSystems[j]->GetCommandBuffer()));
        }

        buffers.push_back(skyboxCubemap.GetComplexCommandBuffer());

        vkCmdExecuteCommands(primaryCmdBuffer, buffers.size(), buffers.data());

        vkCmdEndRenderPass(primaryCmdBuffer);

        if (vkEndCommandBuffer(primaryCmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Scene::RebuildCubeCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(primaryCmdBufferCube, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // std::cout << "Primary Cube Command Buffer: " << primaryCmdBufferCube << std::endl;

        VkRenderPassBeginInfo renderPassInfo {};
        temporaryCubeMapFbo.GetRenderPassBeginInfo(&renderPassInfo);

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBeginRenderPass(primaryCmdBufferCube, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
        std::vector<VkCommandBuffer> buffers;
        for (size_t j = 0; j < models.size(); j++)
        {
            buffers.push_back((models[j]->GetCubeMapCommandBuffer()));
        }

        // std::cout << cubeMapImage.GetImage().GetImage() << std::endl;

        buffers.push_back(skyboxCubemap.GetSimpleCommandBuffer());

        vkCmdExecuteCommands(primaryCmdBufferCube, buffers.size(), buffers.data()); // Broken here

        vkCmdEndRenderPass(primaryCmdBufferCube);

        if (vkEndCommandBuffer(primaryCmdBufferCube) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
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

        RebuildCommandBuffer();

        if (vkAllocateCommandBuffers(*device, &allocInfo, &primaryCmdBufferCube) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        RebuildCubeCommandBuffer();
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

    Entity & Scene::CreateEntity(Model & model, bool isReflectRefractive)
    {
        Entity & entity = model.CreateEntity(isReflectRefractive);
        CreateCommandBuffer();
        entity.LoadLights(lights);
        return entity;
    }

    Light & Scene::CreateLight(bool isSpotlight)
    {
        lights.push_back(new Light());
        lights.back()->CreateLight(isSpotlight);

        return *lights.back();
    }

    GraphicsPipeline & Scene::CreatePipeline(const char * complexVertexFile,
                                             const char * complexFragmentFile,
                                             const char * simpleVertexFile,
                                             const char * simpleFragmentFile)
    {
        pipelines.push_back(new GraphicsPipeline());
        pipelines.back()->CreatePipeline(*device,
                                         renderedImageFbo,
                                         complexVertexFile,
                                         complexFragmentFile,
                                         temporaryCubeMapFbo,
                                         simpleVertexFile,
                                         simpleFragmentFile,
                                         GraphicsPipeline::PipelineType::MODEL_PIPELINE);

        return *pipelines.back();
    }

    Model & Scene::CreateModel(const char *       modelFile,
                               const char *       colorFile,
                               const char *       normalFile,
                               const char *       specularFile,
                               GraphicsPipeline & pipeline)
    {
        models.push_back(new Model());

        models.back()->CreateModel(colorFile,
                                   normalFile,
                                   specularFile,
                                   modelFile,
                                   commandPool,
                                   *device,
                                   *physicalDevice,
                                   pipeline,
                                   descriptorPool,
                                   format,
                                   memory,
                                   *graphicsQueue);
        return *models.back();
    }

    Fbo & Scene::CreateFbo()
    {
        fbos.push_back(new Fbo);
        fbos.back()->CreateFBO(*physicalDevice,
                               *device,
                               memory,
                               *graphicsQueue,
                               format,
                               msaaSamples,
                               swapChainExtent,
                               commandPool,
                               false);
        return *fbos.back();
    }

    Camera & Scene::CreateCamera(float fov, float near, float far)
    {
        cameras.push_back(new Camera());
        cameras.back()->CreateCamera(fov, near, far, swapChainExtent, false);
        return *cameras.back();
    }

    void Scene::SetMSAA(int32_t targetMsaaSamples)
    {
        this->msaaSamples = GetTargetSampleCount(targetMsaaSamples);
        RebuildSwapResources(swapChainExtent);
    }

    void Scene::RenderCubeMap(Entity & transparentEntity)
    {
        for (uint32_t i = 0; i < models.size(); i++)
        {
            models[i]->UpdateCubeCamera(transparentEntity.GetCubeMapCamera());
        }

        skyboxCubemap.UpdateCubeMap(transparentEntity.GetCubeMapCamera());

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount     = 0;
        submitInfo.pWaitDstStageMask      = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &primaryCmdBufferCube;

        vkResetFences(*device, 1, &inFlightFenceCube);

        if (vkQueueSubmit(*graphicsQueue, 1, &submitInfo, inFlightFenceCube) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer! (scene)");
        }
        vkWaitForFences(*device, 1, &inFlightFenceCube, VK_TRUE, UINT64_MAX);

        temporaryCubeMapFbo.CopyToFbo(transparentEntity.GetEnvironmentMapFBO());
    }

    void Scene::Render(Camera & camera, float deltaTime)
    {
        for (uint32_t i = 0; i < models.size(); i++)
        {
            std::vector<Entity *> reflectiveEntities = models[i]->GetReflectiveEntities();
            for (uint32_t j = 0; j < reflectiveEntities.size(); j++)
            {
                RenderCubeMap(*reflectiveEntities[j]);
            }
        }

        for (uint32_t i = 0; i < models.size(); i++)
        {
            models[i]->Update(camera);
        }

        skyboxCubemap.Update(camera);

        vkWaitForFences(*device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        for (uint32_t i = 0; i < particleSystems.size(); i++)
        {
            particleSystems[i]->Update(camera, deltaTime);
        }

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount     = 0;
        submitInfo.pWaitDstStageMask      = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &primaryCmdBuffer;

        vkResetFences(*device, 1, &inFlightFence);

        if (vkQueueSubmit(*graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer! (scene)");
        }
    }

    Fbo & Scene::GetRenderedImage() { return renderedImageFbo; }

    ParticleSystem & Scene::CreateParticleSystem(uint32_t     numStages,
                                                 const char * texture,
                                                 uint32_t     hStages,
                                                 uint32_t     vStages,
                                                 uint32_t     totalStages)
    {
        particleSystems.push_back(new ParticleSystem());
        particleSystems.back()->CreateParticleSystem(texture,
                                                     vStages,
                                                     hStages,
                                                     totalStages,
                                                     commandPool,
                                                     *device,
                                                     *physicalDevice,
                                                     particlePipeline,
                                                     descriptorPool,
                                                     memory,
                                                     *graphicsQueue,
                                                     numStages);
        return *particleSystems.back();
    }

    Filter & Scene::CreateFilter(const char * fragmentFile)
    {
        filters.push_back(new Filter());
        filters.back()->CreateFilter(fragmentFile,
                                     "Resources/fbo_vertex_normal.spv",
                                     *device,
                                     *physicalDevice,
                                     memory,
                                     descriptorPool,
                                     *graphicsQueue,
                                     commandPool,
                                     format,
                                     msaaSamples,
                                     swapChainExtent);
        return *filters.back();
    }

    Combine & Scene::CreateCombine(const char * fragmentFile)
    {
        combines.push_back(new Combine());
        combines.back()->CreateCombine(fragmentFile,
                                       "Resources/fbo_vertex_normal.spv",
                                       *device,
                                       *physicalDevice,
                                       memory,
                                       descriptorPool,
                                       *graphicsQueue,
                                       commandPool,
                                       format,
                                       msaaSamples,
                                       swapChainExtent);
        return *combines.back();
    }

} // namespace sckz