#include "Gui.hpp"

namespace sckz
{
    void Gui::CreateGUI(const char *                 textureFileName,
                        VkCommandPool &              commandPool,
                        VkRenderPass &               renderPass,
                        VkDevice &                   device,
                        VkPhysicalDevice &           physicalDevice,
                        std::vector<VkFramebuffer> & framebuffers,
                        GraphicsPipeline &           pipeline,
                        DescriptorPool &             descriptorPool,
                        VkExtent2D                   swapChainExtent,
                        Memory &                     memory,
                        VkQueue &                    queue)
    {
        this->textureFileName = textureFileName;
        this->renderPass      = &renderPass;
        this->device          = &device;
        this->physicalDevice  = &physicalDevice;
        this->commandPool     = &commandPool;
        this->descriptorPool  = &descriptorPool;
        this->pipeline        = &pipeline;
        this->framebuffers    = &framebuffers;
        this->swapChainExtent = swapChainExtent;
        this->queue           = &queue;
        this->memory          = &memory;

        this->hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                           *this->device,
                                           *this->memory,
                                           0x7FFFFFF,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *this->queue,
                                           commandPool);

        texture.CreateTextureImage(textureFileName, *this->device, *this->physicalDevice, memory, commandPool, queue);

        texture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        texture.CreateTextureSampler();

        CreateUniformBuffers();
        this->pipeline->BindShaderData(&uniformBuffer[0], &texture, nullptr, descriptorPool, &descriptorSet);

        CreateCommandBuffer();
    }

    void Gui::DestroyGUI()
    {
        texture.DestroyImage();
        hostLocalBuffer.DestroyBuffer();
    }

    void Gui::DestroySwapResources()
    {
        vkFreeCommandBuffers(*device, *commandPool, commandBuffers.size(), commandBuffers.data());
    }

    void Gui::RebuildSwapResources(DescriptorPool &             descriptorPool,
                                   VkExtent2D                   swapChainExtent,
                                   VkRenderPass &               renderPass,
                                   std::vector<VkFramebuffer> & framebuffers)
    {
        DestroySwapResources();

        this->framebuffers    = &framebuffers;
        this->descriptorPool  = &descriptorPool;
        this->swapChainExtent = swapChainExtent;
        this->renderPass      = &renderPass;

        CreateUniformBuffers();
        pipeline->BindShaderData(&uniformBuffer[0], &texture, nullptr, descriptorPool, &descriptorSet);
        CreateCommandBuffer();

        Update();
    }

    void Gui::CreateUniformBuffers()
    {
        VkDeviceSize VuboSize = sizeof(VertexUniformBufferObject);
        uniformBuffer[0]      = hostLocalBuffer.GetBuffer(VuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    void Gui::CreateCommandBuffer()
    {
        commandBuffers.resize(framebuffers->size());

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(*device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (uint32_t i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags
                = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

            VkCommandBufferInheritanceInfo inheritanceInfo {};
            inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            inheritanceInfo.renderPass = *renderPass;
            // Secondary command buffer also use the currently active framebuffer
            inheritanceInfo.framebuffer = framebuffers->at(i);

            beginInfo.pInheritanceInfo = &inheritanceInfo;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass        = *renderPass;
            renderPassInfo.framebuffer       = framebuffers->at(i);
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapChainExtent;

            std::array<VkClearValue, 2> clearValues {};
            clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues    = clearValues.data();

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

            vkCmdBindDescriptorSets(commandBuffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline->GetPieplineLayout(),
                                    0,
                                    1,
                                    &descriptorSet,
                                    0,
                                    nullptr);

            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            // Uses the wacky shader from Sascha to draw the image to the screen

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void Gui::Update()
    {
        VertexUniformBufferObject Vubo {};
        Vubo.location.x = location.x / swapChainExtent.width;
        Vubo.location.y = location.y / swapChainExtent.height;
        Vubo.location.z = 0.0;
        Vubo.location.w = 0.0;
        Vubo.scale.x    = scale.x / swapChainExtent.width;
        Vubo.scale.y    = scale.y / swapChainExtent.height;
        Vubo.scale.z    = 0.0;
        Vubo.scale.w    = 0.0;
        Vubo.rotation.x = rotationPoint.x / swapChainExtent.width;
        Vubo.rotation.y = rotationPoint.y / swapChainExtent.height;
        Vubo.rotation.z = glm::radians(rotation);

        uniformBuffer[0].CopyDataToBuffer(&Vubo, sizeof(Vubo));
    }

    void Gui::SetLocation(float x, float y)
    {
        this->location.x = x;
        this->location.y = y;
        Update();
    }

    void Gui::SetRotation(float rotation)
    {
        this->rotation = rotation;
        Update();
    }

    void Gui::SetRotationPoint(float x, float y)
    {
        this->rotationPoint.x = x;
        this->rotationPoint.y = y;
        Update();
    }

    void Gui::SetScale(float x, float y)
    {
        this->scale.x = x;
        this->scale.y = y;
        Update();
    }

    glm::vec2 Gui::GetLocation() { return location; }

    glm::vec2 Gui::GetRotationPoint() { return rotationPoint; }

    float Gui::GetRotation() { return rotation; }

    glm::vec2 Gui::GetScale() { return scale; }

    VkCommandBuffer Gui::GetCommandBuffer(uint32_t index) { return commandBuffers[index]; }
} // namespace sckz