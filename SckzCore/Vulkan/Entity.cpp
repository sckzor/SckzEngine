#include "Entity.hpp"

namespace sckz
{
    void Entity::CreateEntity(VkPhysicalDevice &     physicalDevice,
                              VkDevice &             device,
                              VkQueue &              queue,
                              Buffer &               hostLocalBuffer,
                              DescriptorPool &       descriptorPool,
                              GraphicsPipeline &     pipeline,
                              std::array<Image, 3> & textures)
    {
        this->physicalDevice  = &physicalDevice;
        this->queue           = &queue;
        this->device          = &device;
        this->descriptorPool  = &descriptorPool;
        this->pipeline        = &pipeline;
        this->textures        = &textures;
        this->hostLocalBuffer = &hostLocalBuffer;

        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);

        CreateUniformBuffers();
        this->pipeline->BindShaderData(&uniformBuffer[0],
                                       this->textures->data(),
                                       &uniformBuffer[1],
                                       *this->descriptorPool,
                                       &descriptorSet);
    }

    void Entity::DestroyEntity() { }

    void Entity::RebuildSwapResources()
    {
        DestroyEntity();
        CreateUniformBuffers();
        this->pipeline->BindShaderData(&uniformBuffer[0],
                                       this->textures->data(),
                                       &uniformBuffer[1],
                                       *this->descriptorPool,
                                       &descriptorSet);
    }

    void Entity::SetShine(float reflectivity, float shineDamper)
    {
        this->reflectivity = reflectivity;
        this->shineDamper  = shineDamper;
    }

    void Entity::Update(Camera & camera)
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

        Fubo.extras.x = this->reflectivity;
        Fubo.extras.y = this->shineDamper;

        std::vector<std::pair<Light *, float>> sortedLights = LightSort::GetNearestLights(*lights, location);

        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (i < sortedLights.size())
            {
                Vubo.lightPosition[i] = sortedLights[i].first->GetLocation();
                Fubo.lightColor[i]    = glm::vec4(sortedLights[i].first->GetColor(), 0);
                Fubo.attenuation[i]   = glm::vec4(sortedLights[i].first->GetAttenuation(), 0);
                Fubo.direction[i]
                    = glm::vec4(sortedLights[i].first->GetDirection(), sortedLights[i].first->IsSpotlight() ? 1 : 0);
                Fubo.cutoffs[i].x = glm::radians(sortedLights[i].first->GetCutoff());
                Fubo.cutoffs[i].y = glm::radians(sortedLights[i].first->GetOuterCutoff());
            }
            else
            {
                Vubo.lightPosition[i] = glm::vec3(0, 0, 0);
                Fubo.lightColor[i]    = glm::vec4(0, 0, 0, 0);
                Fubo.attenuation[i]   = glm::vec4(1, 0, 0, 0);
                Fubo.direction[i]     = glm::vec4(0, 0, 0, 0);
            }
        }

        uniformBuffer[0].CopyDataToBuffer(&Vubo, sizeof(Vubo));
        uniformBuffer[1].CopyDataToBuffer(&Fubo, sizeof(Fubo));
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