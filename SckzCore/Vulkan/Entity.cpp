#include "Entity.hpp"

namespace sckz
{
    void Entity::CreateEntity(VkPhysicalDevice & physicalDevice,
                              VkDevice &         device,
                              VkQueue &          queue,
                              Buffer &           hostLocalBuffer,
                              DescriptorPool &   pool,
                              GraphicsPipeline & pipeline,
                              Image &            texture)
    {
        this->physicalDevice  = &physicalDevice;
        this->queue           = &queue;
        this->device          = &device;
        this->pool            = &pool;
        this->pipeline        = &pipeline;
        this->texture         = &texture;
        this->hostLocalBuffer = &hostLocalBuffer;

        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);

        CreateUniformBuffers();
        CreateDescriptorSets();
    }

    void Entity::DestroyEntity() { }

    void Entity::RebuildSwapResources()
    {
        DestroyEntity();
        CreateUniformBuffers();
        CreateDescriptorSets();
    }

    void Entity::SetShine(float reflectivity, float shineDamper)
    {
        this->reflectivity = reflectivity;
        this->shineDamper  = shineDamper;
    }

    void Entity::Update(uint32_t currentImage, Camera & camera)
    {
        VertexUniformBufferObject   Vubo {};
        FragmentUniformBufferObject Fubo {};
        Vubo.model = glm::scale(glm::mat4(1.0f), scale);
        Vubo.model = glm::rotate(Vubo.model, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
        Vubo.model = glm::rotate(Vubo.model, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
        Vubo.model = glm::rotate(Vubo.model, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
        Vubo.model = glm::translate(Vubo.model, location);

        Vubo.view = camera.GetView();
        Vubo.proj = camera.GetProjection();

        Fubo.reflectivity.x = this->reflectivity;
        Fubo.reflectivity.y = this->shineDamper;

        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (i < lights->size())
            {
                Vubo.lightPosition[i] = (*lights)[i]->GetLocation();
                Fubo.lightColor[i]    = glm::vec4((*lights)[i]->GetColor(), 0);
                Fubo.attenuation[i]   = glm::vec4((*lights)[i]->GetAttenuation(), 0);
            }
            else
            {
                Vubo.lightPosition[i] = glm::vec3(0, 0, 0);
                Fubo.lightColor[i]    = glm::vec4(0, 0, 0, 0);
                Fubo.attenuation[i]   = glm::vec4(1, 0, 0, 0);
            }
        }

        uniformBuffer[0].CopyDataToBuffer(&Vubo, sizeof(Vubo));
        uniformBuffer[1].CopyDataToBuffer(&Fubo, sizeof(Fubo));
    }

    void Entity::CreateDescriptorSets()
    {
        VkDescriptorSetLayout       layout(pipeline->GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = pool->GetDescriptorPool();
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
        imageInfo.imageView   = texture->GetImageView();
        imageInfo.sampler     = texture->GetSampler();

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

    void Entity::CreateUniformBuffers()
    {
        VkDeviceSize VuboSize = sizeof(VertexUniformBufferObject);
        VkDeviceSize FuboSize = sizeof(FragmentUniformBufferObject);
        uniformBuffer[0]      = hostLocalBuffer->GetBuffer(VuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        uniformBuffer[1]      = hostLocalBuffer->GetBuffer(FuboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
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

    void Entity::LoadLights(std::vector<Light *> & lights) { this->lights = &lights; }

    glm::vec3 Entity::GetLocation() { return location; }
    glm::vec3 Entity::GetRotation() { return rotation; }
    glm::vec3 Entity::GetScale() { return scale; }

    VkDescriptorSet & Entity::GetDescriptorSet() { return descriptorSet; }
} // namespace sckz