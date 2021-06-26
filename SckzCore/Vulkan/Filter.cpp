#include "Filter.hpp"

namespace sckz
{
    void Filter::CreateFilter(const char *          fragmentFile,
                              const char *          vertexFile,
                              VkDevice &            device,
                              VkPhysicalDevice &    physicalDevice,
                              Memory &              memory,
                              DescriptorPool &      descriptorPool,
                              VkQueue &             graphicsQueue,
                              VkCommandPool &       commandPool,
                              VkFormat              format,
                              VkSampleCountFlagBits msaaSamples,
                              VkExtent2D            swapChainExtent)
    {
        this->device          = &device;
        this->physicalDevice  = &physicalDevice;
        this->memory          = &memory;
        this->descriptorPool  = &descriptorPool;
        this->graphicsQueue   = &graphicsQueue;
        this->commandPool     = &commandPool;
        this->format          = format;
        this->msaaSamples     = msaaSamples;
        this->swapChainExtent = swapChainExtent;

        tempFbo.CreateFBO(physicalDevice, device, memory, graphicsQueue, format, msaaSamples, swapChainExtent);

        filterPipeline.CreatePipeline(*this->device,
                                      tempFbo,
                                      vertexFile,
                                      fragmentFile,
                                      GraphicsPipeline::PipelineType::PARTICLE_PIPELINE);

        CreateCommandBuffer();
    }
    void Filter::DestroyFilter() { }

    void Filter::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer.GetCommandBuffer()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void Filter::RebuildCommandBuffer(Fbo & fbo)
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkCommandBufferInheritanceInfo inheritanceInfo {};
        inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = tempFbo.GetRenderPass();
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = tempFbo.GetImageFramebuffer();

        beginInfo.pInheritanceInfo = &inheritanceInfo;

        if (vkBeginCommandBuffer(commandBuffer.GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        tempFbo.GetRenderPassBeginInfo(renderPassInfo);

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBindPipeline(commandBuffer.GetCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          filterPipeline.GetPipeline());

        VkDescriptorSet ds;

        filterPipeline.BindShaderData(nullptr, &fbo.GetImage(), nullptr, *descriptorPool, &ds);

        vkCmdBindDescriptorSets(commandBuffer.GetCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                filterPipeline.GetPieplineLayout(),
                                0,
                                1,
                                &ds,
                                0,
                                nullptr);

        vkCmdDraw(commandBuffer.GetCommandBuffer(), 3, 1, 0, 0);
        // Uses the wacky shader from Sascha to draw the image to the screen

        if (vkEndCommandBuffer(commandBuffer.GetCommandBuffer()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Filter::Render(Fbo & fbo)
    {
        RebuildCommandBuffer(fbo);
        vkWaitForFences(*device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(*device, 1, &inFlightFence);

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount     = 0;
        submitInfo.pWaitDstStageMask      = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer.GetCommandBuffer();

        submitInfo.signalSemaphoreCount = 0;

        if (vkQueueSubmit(*graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer! (scene)");
        }

        vkQueueWaitIdle(*graphicsQueue);

        tempFbo.CopyToFbo(fbo, *commandPool);
    }

    void Filter::CreateSyncObjects()
    {
        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(*device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

} // namespace sckz