#include "ParticleSystem.hpp"

namespace sckz
{
    void ParticleSystem::CreateParticleSystem(const char *       textureFileName,
                                              uint32_t           vStages,
                                              uint32_t           hStages,
                                              uint32_t           totalStages,
                                              VkCommandPool &    commandPool,
                                              VkDevice &         device,
                                              VkPhysicalDevice & physicalDevice,
                                              GraphicsPipeline & pipeline,
                                              DescriptorPool &   descriptorPool,
                                              Memory &           memory,
                                              VkQueue &          queue,
                                              uint32_t           maxParticles)
    {
        this->textureFileName = textureFileName;
        this->device          = &device;
        this->physicalDevice  = &physicalDevice;
        this->commandPool     = &commandPool;
        this->descriptorPool  = &descriptorPool;
        this->pipeline        = &pipeline;
        this->queue           = &queue;
        this->memory          = &memory;
        this->hStages         = hStages;
        this->vStages         = vStages;
        this->totalStages     = totalStages;

        particles.resize(maxParticles);

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

        lifeLength = 4;

        CreateUniformBuffers();

        for (uint32_t i = 0; i < particles.size(); i++)
        {
            std::array<Buffer::SubBlock, 2> ubos = { staticUniformBuffer, particles[i].uniformBuffer };
            this->pipeline->BindShaderData(ubos.data(), &texture, nullptr, descriptorPool, &particles[i].descriptorSet);
            particles[i].scale = 0;
        }

        CreateCommandBuffer();
    }

    void ParticleSystem::DestroyParticleSystem()
    {
        texture.DestroyImage();
        hostLocalBuffer.DestroyBuffer();
    }

    void ParticleSystem::DestroySwapResources() { vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer); }

    void ParticleSystem::RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent)
    {
        DestroySwapResources();

        this->descriptorPool = &descriptorPool;

        CreateUniformBuffers();
        for (uint32_t i = 0; i < particles.size(); i++)
        {
            std::array<Buffer::SubBlock, 2> ubos = { staticUniformBuffer, particles[i].uniformBuffer };
            this->pipeline->BindShaderData(ubos.data(), &texture, nullptr, descriptorPool, &particles[i].descriptorSet);
        }

        CreateCommandBuffer();
    }

    void ParticleSystem::CreateUniformBuffers()
    {
        VkDeviceSize SVuboSize  = sizeof(StaticVertexUniformBufferObject);
        VkDeviceSize PPVuboSize = sizeof(PerParticleVertexUniformBufferObject);
        staticUniformBuffer     = hostLocalBuffer.GetBuffer(SVuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        for (uint32_t i = 0; i < particles.size(); i++)
        {
            particles[i].uniformBuffer = hostLocalBuffer.GetBuffer(PPVuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        }
    }
    void ParticleSystem::CreateCommandBuffer()
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

        RebuildCommandBuffer();
    }

    void ParticleSystem::RebuildCommandBuffer()
    {

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags
            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

        VkCommandBufferInheritanceInfo inheritanceInfo {};
        inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = pipeline->GetFbo().GetRenderPass();
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = pipeline->GetFbo().GetImageFramebuffer();

        beginInfo.pInheritanceInfo = &inheritanceInfo;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        pipeline->GetFbo().GetRenderPassBeginInfo(&renderPassInfo);

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

        for (uint32_t i = 0; i < particles.size(); i++)
        {
            if (particles[i].scale != 0)
            {
                vkCmdBindDescriptorSets(commandBuffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeline->GetPieplineLayout(),
                                        0,
                                        1,
                                        &particles[i].descriptorSet,
                                        0,
                                        nullptr);

                // Uses the wacky shader from Sascha to draw the image to the screen
                vkCmdDraw(commandBuffer, 3, 1, 0, 0);
            }
        }

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void ParticleSystem::AddParticle(float lx, float ly, float lz, float vx, float vy, float vz)
    {
        for (uint32_t i = 0; i < particles.size(); i++)
        {
            if (particles[i].scale == 0)
            {
                particles[i].location    = glm::vec3(lx, ly, lz);
                particles[i].scale       = 1;
                particles[i].velocity    = glm::vec3(vx, vy, vz);
                particles[i].elapsedTime = 0;
                updated                  = true;
                break;
            }
        }
    }

    void ParticleSystem::Update(Camera & camera, float delataTime)
    {
        StaticVertexUniformBufferObject Subo {};

        if (updated)
        {
            RebuildCommandBuffer();
            updated = false;
        }

        Subo.view = camera.GetView();
        Subo.proj = camera.GetProjection();

        staticUniformBuffer.CopyDataToBuffer(&Subo, sizeof(Subo), 0);

        for (uint32_t i = 0; i < particles.size(); i++)
        {
            if (particles[i].scale == 0)
            // If the scale is zero there is no reason to do complex transfer opperations or math
            {
                continue; // Skip this iteration.
            }

            if (particles[i].elapsedTime <= lifeLength)
            {
                particles[i].velocity.z += GRAVITY * delataTime;

                glm::vec3 change = particles[i].velocity;
                change *= delataTime;
                particles[i].location += change;
                particles[i].elapsedTime += delataTime; //

                UpdateMvpMatrix(particles[i], Subo);
            }
            else
            {
                particles[i].scale = 0;
                UpdateMvpMatrix(particles[i], Subo);
            }
        }
    }

    void ParticleSystem::UpdateMvpMatrix(Particle & particle, StaticVertexUniformBufferObject & Subo)
    {
        PerParticleVertexUniformBufferObject PPubo {};
        PPubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(particle.scale));
        PPubo.model = glm::translate(PPubo.model, particle.location);

        // Create the billboard affect by removing the rotation from the matrix
        PPubo.model[0][0] = Subo.view[0][0];
        PPubo.model[0][1] = Subo.view[1][0];
        PPubo.model[0][2] = Subo.view[2][0];
        PPubo.model[1][0] = Subo.view[0][1];
        PPubo.model[1][1] = Subo.view[1][1];
        PPubo.model[1][2] = Subo.view[2][1];
        PPubo.model[2][0] = Subo.view[0][2];
        PPubo.model[2][1] = Subo.view[1][2];
        PPubo.model[2][2] = Subo.view[2][2];

        PPubo.model = glm::scale(PPubo.model, glm::vec3(particle.scale));

        PPubo.stageData.x = hStages;
        PPubo.stageData.y = vStages;
        PPubo.stageData.z = (hStages * vStages) * (particle.elapsedTime / lifeLength);
        PPubo.stageData.w = totalStages;

        particle.uniformBuffer.CopyDataToBuffer(&PPubo, sizeof(PPubo), 0);
    }

    VkCommandBuffer & ParticleSystem::GetCommandBuffer() { return commandBuffer; }
} // namespace sckz