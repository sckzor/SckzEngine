#include "Entity.hpp"

namespace sckz
{
    void Entity::CreateEntity(VkPhysicalDevice & physicalDevice,
                              VkDevice &         device,
                              VkQueue &          queue,
                              Memory &           memory,
                              DescriptorPool &   pool,
                              GraphicsPipeline & pipeline,
                              uint32_t           numFrameBuffers,
                              Image &            texture)
    {
        this->physicalDevice  = &physicalDevice;
        this->queue           = &queue;
        this->memory          = &memory;
        this->device          = &device;
        this->pool            = &pool;
        this->pipeline        = &pipeline;
        this->numFrameBuffers = numFrameBuffers;
        this->texture         = &texture;

        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);

        CreateUniformBuffers();
        CreateDescriptorSets();
    }

    void Entity::DestroyEntity()
    {
        for (size_t i = 0; i < numFrameBuffers; i++)
        {
            uniformBuffers[i].DestroyBuffer();
        }
    }

    void Entity::RebuildSwapResources()
    {
        DestroyEntity();
        CreateUniformBuffers();
        CreateDescriptorSets();
    }

    void Entity::Update(uint32_t currentImage, Camera & camera)
    {
        UniformBufferObject ubo {};
        ubo.model = glm::scale(glm::mat4(1.0f), scale);
        ubo.model = glm::rotate(ubo.model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        ubo.model = glm::rotate(ubo.model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        ubo.model = glm::rotate(ubo.model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        ubo.model = glm::translate(ubo.model, location);

        ubo.view = camera.GetView();
        ubo.proj = camera.GetProjection();

        uniformBuffers[currentImage].CopyDataToBuffer(&ubo, sizeof(ubo));
    }

    void Entity::CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(numFrameBuffers, pipeline->GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo        allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = pool->GetDescriptorPool();
        allocInfo.descriptorSetCount = static_cast<uint32_t>(numFrameBuffers);
        allocInfo.pSetLayouts        = layouts.data();

        descriptorSets.resize(numFrameBuffers);
        if (vkAllocateDescriptorSets(*device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < numFrameBuffers; i++)
        {
            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = uniformBuffers[i].GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range  = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = texture->GetImageView();
            imageInfo.sampler     = texture->GetSampler();

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

    void Entity::CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(numFrameBuffers);

        for (size_t i = 0; i < numFrameBuffers; i++)
        {
            uniformBuffers[i].CreateBuffer(*physicalDevice,
                                           *device,
                                           *memory,
                                           bufferSize,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *queue);
        }
    }

    void Entity::SetLocation(float x, float y, float z)
    {
        location.x = x;
        location.y = y;
        location.z = z;
    }

    void Entity::SetRotation(float x, float y, float z)
    {
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
    }

    void Entity::SetScale(float x, float y, float z)
    {
        scale.x = x;
        scale.y = y;
        scale.z = z;
    }

    glm::vec3 Entity::GetLocation() { return location; }
    glm::vec3 Entity::GetRotation() { return rotation; }
    glm::vec3 Entity::GetScale() { return scale; }

    std::vector<VkDescriptorSet> & Entity::GetDescriptorSets() { return descriptorSets; }
} // namespace sckz