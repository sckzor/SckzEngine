#include "Combine.hpp"

namespace sckz
{
    void Combine::CreateCombine(const char *          fragmentFile,
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

        tempFbo.CreateFBO(physicalDevice,
                          device,
                          memory,
                          graphicsQueue,
                          format,
                          msaaSamples,
                          swapChainExtent,
                          commandPool,
                          false);

        combinePipeline.CreatePipeline(*this->device,
                                       tempFbo,
                                       vertexFile,
                                       fragmentFile,
                                       tempFbo,
                                       nullptr,
                                       nullptr,
                                       GraphicsPipeline::PipelineType::COMBINE_PIPELINE);

        CreateCommandBuffer();
        CreateSyncObjects();
    }

    void Combine::RebuildSwapResources(DescriptorPool &      descriptorPool,
                                       VkSampleCountFlagBits msaaSamples,
                                       VkExtent2D            swapChainExtent)
    {
        this->msaaSamples     = msaaSamples;
        this->swapChainExtent = swapChainExtent;
        this->descriptorPool  = &descriptorPool;

        tempFbo.RebuildSwapResources(msaaSamples, swapChainExtent);

        combinePipeline.DestroyPipeline();
        combinePipeline.CreatePipeline(*this->device, tempFbo, tempFbo);

        // RebuildCommandBuffer(nullptr, nullptr);

        lastRenderedFbo1 = nullptr;
        lastRenderedFbo2 = nullptr;
    }

    void Combine::DestroyCombine()
    {
        tempFbo.DestroyFBO();
        vkDestroyFence(*device, inFlightFence, nullptr);
        combinePipeline.DestroyPipeline();
        vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
    }

    void Combine::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void Combine::RebuildCommandBuffer(Fbo * fbo1, Fbo * fbo2)
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

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = tempFbo.GetRenderPass();
        renderPassInfo.framebuffer       = tempFbo.GetImageFramebuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, combinePipeline.GetComplexPipeline());

        VkDescriptorSet ds;

        std::array<Image, 2> images = { fbo2->GetImage(), fbo1->GetImage() };

        combinePipeline.BindComplexShaderData(nullptr, images.data(), nullptr, *descriptorPool, &ds);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                combinePipeline.GetComplexPipelineLayout(),
                                0,
                                1,
                                &ds,
                                0,
                                nullptr);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        // Uses the wacky shader from Sascha to draw the image to the screen

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    Fbo & Combine::CombineFbos(Fbo & fbo1, Fbo & fbo2)
    {
        vkWaitForFences(*device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        if (&fbo1 != lastRenderedFbo1 || &fbo2 != lastRenderedFbo2)
        {
            RebuildCommandBuffer(&fbo1, &fbo2);
            lastRenderedFbo1 = &fbo1;
            lastRenderedFbo2 = &fbo2;
        }

        vkResetFences(*device, 1, &inFlightFence);

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount     = 0;
        submitInfo.pWaitDstStageMask      = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;

        submitInfo.signalSemaphoreCount = 0;

        if (vkQueueSubmit(*graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer! (scene)");
        }

        return tempFbo;
    }

    void Combine::CreateSyncObjects()
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