#include "Model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Loaders/tiny_obj_loader.h"

namespace sckz
{
    void Model::CreateModel(const char *                 textureFileName,
                            const char *                 modelFileName,
                            VkCommandPool &              commandPool,
                            VkRenderPass &               renderPass,
                            VkDevice &                   device,
                            VkPhysicalDevice &           physicalDevice,
                            std::vector<VkFramebuffer> & swapChainFramebuffers,
                            GraphicsPipeline *           pipeline,
                            DescriptorPool &             descriptorPool,
                            VkExtent2D                   swapChainExtent,
                            uint32_t                     numSwapChainImages,
                            VkQueue &                    queue)
    {
        this->textureFileName       = textureFileName;
        this->modelFileName         = modelFileName;
        this->renderPass            = &renderPass;
        this->device                = &device;
        this->physicalDevice        = &physicalDevice;
        this->commandPool           = &commandPool;
        this->descriptorPool        = &descriptorPool;
        this->pipeline              = pipeline;
        this->swapChainFramebuffers = &swapChainFramebuffers;
        this->swapChainExtent       = swapChainExtent;
        this->numSwapChainImages    = numSwapChainImages;
        this->queue                 = &queue;

        this->hasCommandBuffer = false;

        texture.CreateTextureImage(textureFileName, *this->device, *this->physicalDevice, commandPool, queue);
        texture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        texture.CreateTextureSampler();

        LoadModel();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateUniformBuffers();

        CreateDescriptorSets();
    }

    void Model::DestroyModel()
    {
        vertexBuffer.DestroyBuffer();
        indexBuffer.DestroyBuffer();
        texture.DestroyImage();
    }

    void Model::CreateSwapResources(DescriptorPool & descriptorPool, VkExtent2D swapChainExtent)
    {
        this->descriptorPool  = &descriptorPool;
        this->swapChainExtent = swapChainExtent;

        CreateUniformBuffers();
        CreateDescriptorSets();
        CreateCommandBuffers();
    }

    void Model::DestroySwapResources()
    {
        for (size_t i = 0; i < numSwapChainImages; i++)
        {
            uniformBuffers[i].DestroyBuffer();
        }

        vkFreeCommandBuffers(*device,
                             *commandPool,
                             static_cast<uint32_t>(commandBuffers.size()),
                             commandBuffers.data());
    }

    void Model::Update(uint32_t currentImage)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto  currentTime = std::chrono::high_resolution_clock::now();
        float time        = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo {};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, time * 0.1f));
        ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj
            = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        uniformBuffers[currentImage].CopyDataToBuffer(&ubo, sizeof(ubo));
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

                vertex.color = { 1.0f, 1.0f, 1.0f };

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

        Buffer stagingBuffer;
        stagingBuffer.CreateBuffer(*physicalDevice,
                                   *device,
                                   bufferSize,
                                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   *queue);

        stagingBuffer.CopyDataToBuffer(vertices.data(), bufferSize);

        vertexBuffer.CreateBuffer(*physicalDevice,
                                  *device,
                                  bufferSize,
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  *queue);

        stagingBuffer.CopyBufferToBuffer(vertexBuffer, *commandPool);

        stagingBuffer.DestroyBuffer();
    }

    void Model::CreateIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        Buffer stagingBuffer;
        stagingBuffer.CreateBuffer(*physicalDevice,
                                   *device,
                                   bufferSize,
                                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   *queue);

        stagingBuffer.CopyDataToBuffer(indices.data(), bufferSize);

        indexBuffer.CreateBuffer(*physicalDevice,
                                 *device,
                                 bufferSize,
                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 *queue);

        stagingBuffer.CopyBufferToBuffer(indexBuffer, *commandPool);

        stagingBuffer.DestroyBuffer();
    }

    void Model::CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(numSwapChainImages);

        for (size_t i = 0; i < numSwapChainImages; i++)
        {
            uniformBuffers[i].CreateBuffer(*physicalDevice,
                                           *device,
                                           bufferSize,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *queue);
        }
    }

    void Model::CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(swapChainFramebuffers->size(), pipeline->GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo        allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = descriptorPool->GetDescriptorPool();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainFramebuffers->size());
        allocInfo.pSetLayouts        = layouts.data();

        descriptorSets.resize(swapChainFramebuffers->size());
        if (vkAllocateDescriptorSets(*device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < swapChainFramebuffers->size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = uniformBuffers[i].GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range  = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = texture.GetImageView();
            imageInfo.sampler     = texture.GetSampler();

            std::array<VkWriteDescriptorSet, 2> descriptorWrites {};

            descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet          = descriptorSets[i];
            descriptorWrites[0].dstBinding      = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo     = &bufferInfo;

            descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet          = descriptorSets[i];
            descriptorWrites[1].dstBinding      = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo      = &imageInfo;

            vkUpdateDescriptorSets(*device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(),
                                   0,
                                   nullptr);
        }
    }

    void Model::CreateCommandBuffers()
    {
        commandBuffers.resize(swapChainFramebuffers->size());

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(*device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

            VkCommandBufferInheritanceInfo inheritanceInfo {};
            inheritanceInfo.sType      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            inheritanceInfo.renderPass = *renderPass;
            // Secondary command buffer also use the currently active framebuffer
            inheritanceInfo.framebuffer = (*swapChainFramebuffers)[i];

            beginInfo.pInheritanceInfo = &inheritanceInfo;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo {};
            renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass        = *renderPass;
            renderPassInfo.framebuffer       = (*swapChainFramebuffers)[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapChainExtent;

            std::array<VkClearValue, 2> clearValues {};
            clearValues[0].color        = { 0.0f, 0.0f, 0.0f, 1.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues    = clearValues.data();

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

            VkBuffer     rawVertexBuffer[1];
            VkDeviceSize offsets[] = { 0 };

            rawVertexBuffer[0] = vertexBuffer.GetBuffer();

            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, rawVertexBuffer, offsets);

            vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(commandBuffers[i],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    pipeline->GetPieplineLayout(),
                                    0,
                                    1,
                                    &descriptorSets[i],
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    std::vector<VkCommandBuffer> & Model::GetCommandBuffers()
    {
        if (!hasCommandBuffer)
        {
            CreateCommandBuffers();
        }

        hasCommandBuffer = true;
        return commandBuffers;
    }

    Buffer Model::GetIndexBuffer() { return indexBuffer; }

    Buffer Model::GetVertexBuffer() { return vertexBuffer; }

    std::vector<VkDescriptorSet> & Model::GetDescriptorSets() { return descriptorSets; }

    uint32_t Model::GetNumIndices() { return static_cast<uint32_t>(indices.size()); }
} // namespace sckz