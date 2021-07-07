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
                                DescriptorPool &   descriptorPool)
    {
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->memory         = &memory;
        this->pool           = &pool;
        this->queue          = &queue;
        this->descriptorPool = &descriptorPool;
        this->pipeline       = &pipeline;

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

        LoadModel();

        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateUniformBuffer();

        this->pipeline->BindShaderData(&uniformBuffer, &cubeMapTexture, nullptr, descriptorPool, &descriptorSet);
        CreateCommandBuffer();
    }

    void CubeMap::DestroyCubeMap() { }

    VkCommandBuffer & CubeMap::GetCommandBuffer() { return commandBuffer; }

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

    void CubeMap::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *pool;
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

        VkBuffer     rawVertexBuffer[1];
        VkDeviceSize offsets[1];

        rawVertexBuffer[0] = vertexBuffer->parent->buffer;
        offsets[0]         = vertexBuffer->offset;

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, rawVertexBuffer, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->parent->buffer, indexBuffer->offset, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline->GetPieplineLayout(),
                                0,
                                1,
                                &descriptorSet,
                                0,
                                nullptr);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void CubeMap::Update(glm::vec3 location, Camera & camera)
    {
        VertexUniformBufferObject Vubo {};
        // Vubo.model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
        Vubo.view = camera.GetView();
        Vubo.proj = camera.GetProjection();
        uniformBuffer.CopyDataToBuffer(&Vubo, sizeof(Vubo), 0);
    }

    void CubeMap::CreateUniformBuffer()
    {
        VkDeviceSize VuboSize = sizeof(VertexUniformBufferObject);
        uniformBuffer         = hostLocalBuffer.GetBuffer(VuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }

    void CubeMap::LoadModel()
    {
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string                      warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "Resources/cubemap.obj"))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices {};

        for (const auto & shape : shapes)
        {
            for (const auto & index : shape.mesh.indices)
            {
                Vertex vertex {};

                vertex.pos = { attrib.vertices[3 * index.vertex_index + 0] * 50,
                               attrib.vertices[3 * index.vertex_index + 1] * 50,
                               attrib.vertices[3 * index.vertex_index + 2] * 50 };

                vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
                                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

                vertex.normal = { attrib.normals[3 * index.normal_index + 0],
                                  attrib.normals[3 * index.normal_index + 1],
                                  attrib.normals[3 * index.normal_index + 2] };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }
} // namespace sckz