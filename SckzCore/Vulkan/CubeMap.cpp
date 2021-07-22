#include "CubeMap.hpp"

namespace sckz
{
    void CubeMap::CreateCubeMap(const char *       front,
                                const char *       back,
                                const char *       up,
                                const char *       down,
                                const char *       right,
                                const char *       left,
                                VkDevice &         device,
                                VkPhysicalDevice & physicalDevice,
                                Memory &           memory,
                                VkCommandPool &    pool,
                                VkQueue &          queue,
                                GraphicsPipeline & pipeline,
                                DescriptorPool &   descriptorPool,
                                float              size)
    {
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->memory         = &memory;
        this->pool           = &pool;
        this->queue          = &queue;
        this->descriptorPool = &descriptorPool;
        this->pipeline       = &pipeline;
        this->size           = size;

        for (uint32_t i = 0; i < vertices.size(); i++)
        {
            vertices[i].pos.x *= size;
            vertices[i].pos.y *= size;
            vertices[i].pos.z *= size;
        }

        cubeMapTexture
            .CreateCubeTextureImage(front, back, up, down, right, left, device, physicalDevice, memory, pool, queue);
        cubeMapTexture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        cubeMapTexture.CreateTextureSampler();

        this->hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                           *this->device,
                                           *this->memory,
                                           0x7FFFFFF,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *this->queue,
                                           pool);

        this->deviceLocalBuffer.CreateBuffer(*this->physicalDevice,
                                             *this->device,
                                             *this->memory,
                                             0x7FFFFFF,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                             *this->queue,
                                             pool);

        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateUniformBuffer();

        this->pipeline->BindSimpleShaderData(&simpleUniformBuffer,
                                             &cubeMapTexture,
                                             descriptorPool,
                                             &simpleDescriptorSet);

        this->pipeline->BindComplexShaderData(&complexUniformBuffer,
                                              &cubeMapTexture,
                                              nullptr,
                                              descriptorPool,
                                              &complexDescriptorSet);

        CreateComplexCommandBuffer();
        CreateSimpleCommandBuffer();
    }

    void CubeMap::DestroyCubeMap()
    {
        vkFreeCommandBuffers(*device, *pool, 1, &simpleCommandBuffer);
        vkFreeCommandBuffers(*device, *pool, 1, &complexCommandBuffer);
        hostLocalBuffer.DestroyBuffer();
        deviceLocalBuffer.DestroyBuffer();
        cubeMapTexture.DestroyImage();
    }

    VkCommandBuffer & CubeMap::GetComplexCommandBuffer() { return complexCommandBuffer; }

    VkCommandBuffer & CubeMap::GetSimpleCommandBuffer() { return simpleCommandBuffer; }

    void CubeMap::CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        Buffer::SubBlock stagingBuffer;
        stagingBuffer = hostLocalBuffer.GetBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        stagingBuffer.CopyDataToBuffer((void *)vertices.data(), bufferSize, 0);

        vertexBuffer
            = &deviceLocalBuffer.GetBuffer(bufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        stagingBuffer.CopyBufferToBuffer(*vertexBuffer);

        stagingBuffer.DestroySubBlock();
    }

    void CubeMap::CreateIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        Buffer::SubBlock stagingBuffer;
        stagingBuffer = hostLocalBuffer.GetBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        stagingBuffer.CopyDataToBuffer(indices.data(), bufferSize, 0);

        indexBuffer = &deviceLocalBuffer.GetBuffer(bufferSize,
                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

        stagingBuffer.CopyBufferToBuffer(*indexBuffer);

        stagingBuffer.DestroySubBlock();
    }

    void CubeMap::CreateComplexCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *pool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &complexCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

        VkCommandBufferInheritanceInfo inheritanceInfo {};
        inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = pipeline->GetComplexFbo().GetRenderPass();
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = pipeline->GetComplexFbo().GetImageFramebuffer();

        beginInfo.pInheritanceInfo = &inheritanceInfo;

        if (vkBeginCommandBuffer(complexCommandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        pipeline->GetComplexFbo().GetRenderPassBeginInfo(&renderPassInfo);

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBindPipeline(complexCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetComplexPipeline());

        VkBuffer     rawVertexBuffer[1];
        VkDeviceSize offsets[1];

        rawVertexBuffer[0] = vertexBuffer->parent->buffer;
        offsets[0]         = vertexBuffer->offset;

        vkCmdBindVertexBuffers(complexCommandBuffer, 0, 1, rawVertexBuffer, offsets);
        vkCmdBindIndexBuffer(complexCommandBuffer,
                             indexBuffer->parent->buffer,
                             indexBuffer->offset,
                             VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(complexCommandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline->GetComplexPipelineLayout(),
                                0,
                                1,
                                &complexDescriptorSet,
                                0,
                                nullptr);

        vkCmdDrawIndexed(complexCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        if (vkEndCommandBuffer(complexCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CubeMap::CreateSimpleCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *pool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &simpleCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

        VkCommandBufferInheritanceInfo inheritanceInfo {};
        inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = pipeline->GetSimpleFbo().GetRenderPass();
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = pipeline->GetSimpleFbo().GetImageFramebuffer();

        beginInfo.pInheritanceInfo = &inheritanceInfo;

        if (vkBeginCommandBuffer(simpleCommandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        pipeline->GetSimpleFbo().GetRenderPassBeginInfo(&renderPassInfo);

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues    = clearValues.data();

        vkCmdBindPipeline(simpleCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetSimplePipeline());

        VkBuffer     rawVertexBuffer[1];
        VkDeviceSize offsets[1];

        rawVertexBuffer[0] = vertexBuffer->parent->buffer;
        offsets[0]         = vertexBuffer->offset;

        vkCmdBindVertexBuffers(simpleCommandBuffer, 0, 1, rawVertexBuffer, offsets);
        vkCmdBindIndexBuffer(simpleCommandBuffer,
                             indexBuffer->parent->buffer,
                             indexBuffer->offset,
                             VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(simpleCommandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline->GetSimplePieplineLayout(),
                                0,
                                1,
                                &simpleDescriptorSet,
                                0,
                                nullptr);

        vkCmdDrawIndexed(simpleCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        if (vkEndCommandBuffer(simpleCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CubeMap::Update(glm::vec3 location, Camera & camera)
    {
        ComplexVertexUniformBufferObject Vubo {};
        // Vubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
        Vubo.view = camera.GetView();
        Vubo.proj = camera.GetProjection();

        Vubo.view[3][0] = 0;
        Vubo.view[3][1] = 0;
        Vubo.view[3][2] = 0;
        complexUniformBuffer.CopyDataToBuffer(&Vubo, sizeof(Vubo), 0);

        SimpleVertexUniformBufferObject SVubo {};

        SVubo.proj = camera.GetCubeMapProjection();
        for (uint32_t i = 0; i < CUBEMAP_SIDES; i++)
        {
            SVubo.view[i]       = camera.GetCubeMapView(1, location);
            SVubo.view[i][3][0] = 0;
            SVubo.view[i][3][1] = 0;
            SVubo.view[i][3][2] = 0;
        }

        simpleUniformBuffer.CopyDataToBuffer(&SVubo, sizeof(SVubo), 0);
    }

    void CubeMap::CreateUniformBuffer()
    {
        VkDeviceSize VuboSize = sizeof(ComplexVertexUniformBufferObject);
        complexUniformBuffer  = hostLocalBuffer.GetBuffer(VuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

        VkDeviceSize SVuboSize = sizeof(SimpleVertexUniformBufferObject);
        simpleUniformBuffer    = hostLocalBuffer.GetBuffer(SVuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    Image & CubeMap::GetImage() { return cubeMapTexture; }
} // namespace sckz