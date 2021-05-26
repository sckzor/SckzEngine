#include "Gui.hpp"

namespace sckz
{
    void Gui::CreateGUI(const char *       textureFileName,
                        VkCommandPool &    commandPool,
                        VkRenderPass &     renderPass,
                        VkDevice &         device,
                        VkPhysicalDevice & physicalDevice,
                        VkFramebuffer &    framebuffer,
                        GraphicsPipeline * pipeline,
                        DescriptorPool &   descriptorPool,
                        VkExtent2D         swapChainExtent,
                        Memory &           memory,
                        VkQueue &          queue)
    {
        this->textureFileName = textureFileName;
        this->renderPass      = &renderPass;
        this->device          = &device;
        this->physicalDevice  = &physicalDevice;
        this->commandPool     = &commandPool;
        this->descriptorPool  = &descriptorPool;
        this->pipeline        = pipeline;
        this->framebuffer     = &framebuffer;
        this->swapChainExtent = swapChainExtent;
        this->queue           = &queue;
        this->memory          = &memory;

        this->hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                           *this->device,
                                           *this->memory,
                                           0x7FFFFFF,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *this->queue);

        texture.CreateTextureImage(textureFileName, *this->device, *this->physicalDevice, memory, commandPool, queue);

        texture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        texture.CreateTextureSampler();

        CreateCommandBuffer();
        CreateDescriptorSets();
        CreateUniformBuffers();
    }

    void Gui::DestroyGUI()
    {
        texture.DestroyImage();
        hostLocalBuffer.DestroyBuffer();
    }

    void Gui::RebuildSwapResources() { }

    void Gui::CreateUniformBuffers()
    {
        VkDeviceSize VuboSize = sizeof(VertexUniformBufferObject);
        VkDeviceSize FuboSize = sizeof(FragmentUniformBufferObject);
        uniformBuffer[0]      = hostLocalBuffer.GetBuffer(VuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        uniformBuffer[1]      = hostLocalBuffer.GetBuffer(FuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
    void Gui::CreateDescriptorSets()
    {
        VkDescriptorSetLayout       layout(pipeline->GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = descriptorPool->GetDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &layout;

        if (vkAllocateDescriptorSets(*device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        VkDescriptorBufferInfo VInfo {};
        VInfo.buffer = uniformBuffer[0].parent->buffer;
        VInfo.offset = uniformBuffer[0].offset;
        VInfo.range  = sizeof(VertexUniformBufferObject);

        VkDescriptorBufferInfo FInfo {};
        FInfo.buffer = uniformBuffer[1].parent->buffer;
        FInfo.offset = uniformBuffer[1].offset;
        FInfo.range  = sizeof(FragmentUniformBufferObject);

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = texture.GetImageView();
        imageInfo.sampler     = texture.GetSampler();

        std::array<VkWriteDescriptorSet, 3> descriptorWrites {};

        descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet          = descriptorSet;
        descriptorWrites[0].dstBinding      = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo     = &VInfo;

        descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet          = descriptorSet;
        descriptorWrites[1].dstBinding      = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo      = &imageInfo;

        descriptorWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet          = descriptorSet;
        descriptorWrites[2].dstBinding      = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo     = &FInfo;

        vkUpdateDescriptorSets(*device,
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0,
                               nullptr);
    }

    void Gui::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

        VkCommandBufferInheritanceInfo inheritanceInfo {};
        inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = *renderPass;
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = *framebuffer;

        beginInfo.pInheritanceInfo = &inheritanceInfo;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = *renderPass;
        renderPassInfo.framebuffer       = *framebuffer;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

        VkDescriptorSet ds;

        pipeline->BindImage(texture, *descriptorPool, &ds);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline->GetPieplineLayout(),
                                0,
                                1,
                                &ds,
                                0,
                                nullptr);

        vkCmdDraw(commandBuffer, 3, 1, 0,
                  0); // Uses the wacky shader from Sascha to draw the image to the screen

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Gui::SetLocation(float x, float y)
    {
        this->location.x = x;
        this->location.y = y;
    }

    void Gui::SetRotation(float x) { this->rotation = rotation; }

    void Gui::SetScale(float x, float y)
    {
        this->scale.x = x;
        this->scale.y = y;
    }

    glm::vec2 Gui::GetLocation() { return location; }

    float Gui::GetRotation() { return rotation; }

    glm::vec2 Gui::GetScale() { return scale; }

    VkCommandBuffer Gui::GetCommandBuffer() { return commandBuffer; }
} // namespace sckz