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
            uniformBuffers[i][0].DestroyBuffer();
            uniformBuffers[i][1].DestroyBuffer();
        }
    }

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
                std::cout << (*lights)[i]->GetColor().x << " " << (*lights)[i]->GetColor().y << " "
                          << (*lights)[i]->GetColor().z << " " << std::endl;
                Vubo.lightPosition[i] = (*lights)[i]->GetLocation();
                Fubo.lightColor[i]    = glm::vec4((*lights)[i]->GetColor(), 0);
                Fubo.attenuation[i]   = glm::vec4((*lights)[i]->GetColor(), 0);
            }
            else
            {
                Vubo.lightPosition[i] = glm::vec3(0, 0, 0);
                Fubo.lightColor[i]    = glm::vec4(0, 0, 0, 0);
                Fubo.attenuation[i]   = glm::vec4(1, 0, 0, 0);
            }
        }

        uniformBuffers[currentImage][0].CopyDataToBuffer(&Vubo, sizeof(Vubo));
        uniformBuffers[currentImage][1].CopyDataToBuffer(&Fubo, sizeof(Fubo));
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
            VkDescriptorBufferInfo VInfo {};
            VInfo.buffer = uniformBuffers[i][0].GetBuffer();
            VInfo.offset = 0;
            VInfo.range  = sizeof(VertexUniformBufferObject);

            VkDescriptorBufferInfo FInfo {};
            FInfo.buffer = uniformBuffers[i][1].GetBuffer();
            FInfo.offset = 0;
            FInfo.range  = sizeof(FragmentUniformBufferObject);

            VkDescriptorImageInfo imageInfo {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = texture->GetImageView();
            imageInfo.sampler     = texture->GetSampler();

            std::array<VkWriteDescriptorSet, 3> descriptorWrites {};

            descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet          = descriptorSets[i];
            descriptorWrites[0].dstBinding      = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo     = &VInfo;

            descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet          = descriptorSets[i];
            descriptorWrites[1].dstBinding      = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo      = &imageInfo;

            descriptorWrites[2].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet          = descriptorSets[i];
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
    }

    void Entity::CreateUniformBuffers()
    {
        VkDeviceSize VuboSize = sizeof(VertexUniformBufferObject);
        VkDeviceSize FuboSize = sizeof(FragmentUniformBufferObject);

        uniformBuffers.resize(numFrameBuffers);

        for (size_t i = 0; i < numFrameBuffers; i++)
        {
            uniformBuffers[i][0].CreateBuffer(*physicalDevice,
                                              *device,
                                              *memory,
                                              VuboSize,
                                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                                  | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                              *queue);

            uniformBuffers[i][1].CreateBuffer(*physicalDevice,
                                              *device,
                                              *memory,
                                              FuboSize,
                                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                                  | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
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

    void Entity::LoadLights(std::vector<Light *> & lights) { this->lights = &lights; }

    glm::vec3 Entity::GetLocation() { return location; }
    glm::vec3 Entity::GetRotation() { return rotation; }
    glm::vec3 Entity::GetScale() { return scale; }

    std::vector<VkDescriptorSet> & Entity::GetDescriptorSets() { return descriptorSets; }
} // namespace sckz