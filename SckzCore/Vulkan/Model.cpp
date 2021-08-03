#include "Model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Loaders/tiny_obj_loader.h"

namespace sckz
{
    void Model::CreateModel(const char *       colorFileName,
                            const char *       normalFileName,
                            const char *       spacularFileName,
                            const char *       modelFileName,
                            VkCommandPool &    commandPool,
                            VkDevice &         device,
                            VkPhysicalDevice & physicalDevice,
                            GraphicsPipeline & pipeline,
                            DescriptorPool &   descriptorPool,
                            VkFormat           format,
                            Memory &           memory,
                            VkQueue &          queue)
    {
        this->colorFileName    = colorFileName;
        this->normalFileName   = normalFileName;
        this->spacularFileName = spacularFileName;
        this->modelFileName    = modelFileName;
        this->device           = &device;
        this->physicalDevice   = &physicalDevice;
        this->commandPool      = &commandPool;
        this->descriptorPool   = &descriptorPool;
        this->pipeline         = &pipeline;
        this->queue            = &queue;
        this->memory           = &memory;
        this->format           = format;

        this->hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                           *this->device,
                                           *this->memory,
                                           0x7FFFFFF,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *this->queue,
                                           commandPool);

        this->deviceLocalBuffer.CreateBuffer(*this->physicalDevice,
                                             *this->device,
                                             *this->memory,
                                             0x7FFFFFF,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                             *this->queue,
                                             commandPool);

        blankTexture.CreateBlankTextureImage(*this->device, *this->physicalDevice, memory, commandPool, queue);
        blankTexture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        blankTexture.CreateTextureSampler();

        blankTextureCube.CreateBlankCubeTextureImage(*this->device, *this->physicalDevice, memory, commandPool, queue);
        blankTextureCube.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        blankTextureCube.CreateTextureSampler();

        CreateTexture(textures[0], colorFileName);
        CreateTexture(textures[1], normalFileName);
        CreateTexture(textures[2], spacularFileName);

        LoadModel();
        CreateVertexBuffer();
        CreateIndexBuffer();

        CreateCommandBuffer();
        CreateCubeMapCommandBuffer();
    }

    void Model::CreateTexture(Image & image, const char * fileName)
    {
        if (fileName == nullptr)
        {
            image = blankTexture;
        }
        else
        {
            image.CreateTextureImage(fileName, *this->device, *this->physicalDevice, *memory, *commandPool, *queue);
            image.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
            image.CreateTextureSampler();
        }
    }

    void Model::DestroyModel()
    {
        for (uint32_t i = 0; i < entities.size(); i++)
        {
            entities[i]->DestroyEntity();
            delete entities[i];
        }
        DestroySwapResources();

        vertexBuffer->DestroySubBlock();
        indexBuffer->DestroySubBlock();

        hostLocalBuffer.DestroyBuffer();
        deviceLocalBuffer.DestroyBuffer();
        for (uint32_t i = 0; i < textures.size() - 1; i++)
        {
            if (textures[i] != blankTexture)
            {

                textures[i].DestroyImage();
            }
        }
        blankTexture.DestroyImage();
        blankTextureCube.DestroyImage();
    }

    void Model::Update(Camera & camera)
    {

        for (uint32_t i = 0; i < entities.size(); i++)
        {
            entities[i]->Update(camera);
        }
    }

    std::vector<Entity *> Model::GetReflectiveEntities()
    {
        std::vector<Entity *> reflectiveEntities;
        for (uint32_t i = 0; i < entities.size(); i++)
        {
            if (entities[i]->IsReflective())
            {
                reflectiveEntities.push_back(entities[i]);
            }
        }

        return reflectiveEntities;
    }

    void Model::UpdateCubeCamera(Camera & camera)
    {

        for (uint32_t i = 0; i < entities.size(); i++)
        {
            entities[i]->UpdateCubeMap(camera);
        }
    }

    void Model::LoadModel()
    {
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string                      warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFileName))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices {};

        for (const auto & shape : shapes)
        {
            for (const auto & index : shape.mesh.indices)
            {
                Vertex vertex {};

                vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2] };

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

    void Model::CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        Buffer::SubBlock stagingBuffer;
        stagingBuffer = hostLocalBuffer.GetBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        stagingBuffer.CopyDataToBuffer(vertices.data(), bufferSize, 0);

        vertexBuffer
            = &deviceLocalBuffer.GetBuffer(bufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        stagingBuffer.CopyBufferToBuffer(*vertexBuffer);

        stagingBuffer.DestroySubBlock();
    }

    void Model::CreateIndexBuffer()
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

    void Model::RebuildSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent)
    {
        DestroySwapResources();

        blankTexture.DestroyImage();
        blankTexture.CreateBlankTextureImage(*this->device, *this->physicalDevice, *memory, *commandPool, *queue);
        blankTexture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        blankTexture.CreateTextureSampler();

        blankTextureCube.DestroyImage();
        blankTextureCube.CreateBlankCubeTextureImage(*this->device,
                                                     *this->physicalDevice,
                                                     *memory,
                                                     *commandPool,
                                                     *queue);
        blankTextureCube.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        blankTextureCube.CreateTextureSampler();

        for (size_t i = 0; i < entities.size(); i++)
        {
            entities[i]->RebuildSwapResources(swapChainExtent);
        }
        // Create

        this->descriptorPool = &descriptorPool;

        CreateCommandBuffer();
        CreateCubeMapCommandBuffer();
    }

    void Model::CreateCommandBuffer()
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
        inheritanceInfo.renderPass = pipeline->GetComplexFbo().GetRenderPass();
        // Secondary command buffer also use the currently active framebuffer
        inheritanceInfo.framebuffer = pipeline->GetComplexFbo().GetImageFramebuffer();

        beginInfo.pInheritanceInfo = &inheritanceInfo;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
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

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetComplexPipeline());

        VkBuffer     rawVertexBuffer[1];
        VkDeviceSize offsets[1];

        rawVertexBuffer[0] = vertexBuffer->parent->buffer;
        offsets[0]         = vertexBuffer->offset;

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, rawVertexBuffer, offsets);

        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->parent->buffer, indexBuffer->offset, VK_INDEX_TYPE_UINT32);

        for (uint32_t j = 0; j < entities.size(); j++)
        {
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline->GetComplexPipelineLayout(),
                                    0,
                                    1,
                                    &entities[j]->GetComplexDescriptorSet(),
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        }

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Model::CreateCubeMapCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &cubeMapCommandBuffer) != VK_SUCCESS)
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

        if (vkBeginCommandBuffer(cubeMapCommandBuffer, &beginInfo) != VK_SUCCESS)
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

        vkCmdBindPipeline(cubeMapCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetSimplePipeline());

        VkBuffer     rawVertexBuffer[1];
        VkDeviceSize offsets[1];

        rawVertexBuffer[0] = vertexBuffer->parent->buffer;
        offsets[0]         = vertexBuffer->offset;

        vkCmdBindVertexBuffers(cubeMapCommandBuffer, 0, 1, rawVertexBuffer, offsets);

        vkCmdBindIndexBuffer(cubeMapCommandBuffer,
                             indexBuffer->parent->buffer,
                             indexBuffer->offset,
                             VK_INDEX_TYPE_UINT32);

        for (uint32_t j = 0; j < entities.size(); j++)
        {
            vkCmdBindDescriptorSets(cubeMapCommandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline->GetSimplePieplineLayout(),
                                    0,
                                    1,
                                    &entities[j]->GetSimpleDescriptorSet(),
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(cubeMapCommandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        }

        if (vkEndCommandBuffer(cubeMapCommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    VkCommandBuffer & Model::GetCommandBuffer() { return commandBuffer; }
    VkCommandBuffer & Model::GetCubeMapCommandBuffer() { return cubeMapCommandBuffer; }

    Entity & Model::CreateEntity(bool isReflectRefractive)
    {
        Entity * entity = new Entity();

        entity->CreateEntity(*physicalDevice,
                             *device,
                             *queue,
                             hostLocalBuffer,
                             *descriptorPool,
                             *pipeline,
                             *memory,
                             format,
                             *commandPool,
                             isReflectRefractive,
                             textures,
                             blankTextureCube);

        entities.push_back(entity);
        CreateCommandBuffer();
        CreateCubeMapCommandBuffer();

        return *entity;
    }

    Buffer::SubBlock Model::GetIndexBuffer() { return *indexBuffer; }

    Buffer::SubBlock Model::GetVertexBuffer() { return *vertexBuffer; }

    uint32_t Model::GetNumIndices() { return static_cast<uint32_t>(indices.size()); }

    void Model::DestroySwapResources()
    {
        vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
        vkFreeCommandBuffers(*device, *commandPool, 1, &cubeMapCommandBuffer);
    }
} // namespace sckz