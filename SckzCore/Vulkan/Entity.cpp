#include "Entity.hpp"

namespace sckz
{
    void Entity::CreateEntity(VkPhysicalDevice &     physicalDevice,
                              VkDevice &             device,
                              VkQueue &              queue,
                              Buffer &               hostLocalBuffer,
                              DescriptorPool &       descriptorPool,
                              GraphicsPipeline &     pipeline,
                              Memory &               memory,
                              VkFormat               format,
                              VkCommandPool &        commandPool,
                              bool                   isCubeMap,
                              std::array<Image, 4> & textures)
    {
        this->physicalDevice  = &physicalDevice;
        this->queue           = &queue;
        this->device          = &device;
        this->descriptorPool  = &descriptorPool;
        this->pipeline        = &pipeline;
        this->textures        = &textures;
        this->hostLocalBuffer = &hostLocalBuffer;

        this->scale = glm::vec3(1.0f, 1.0f, 1.0f);

        if (isCubeMap)
        {
            reflectionMap.CreateFBO(physicalDevice,
                                    device,
                                    memory,
                                    queue,
                                    format,
                                    VK_SAMPLE_COUNT_1_BIT,
                                    { 0, 0 },
                                    commandPool,
                                    true);

            cubeMapCamera.CreateCamera(90, 1, 100, { 0, 0 }, true);
        }

        CreateUniformBuffers();

        this->pipeline->BindComplexShaderData(&complexUniformBuffers[0],
                                              textures.data(),
                                              &complexUniformBuffers[1],
                                              *this->descriptorPool,
                                              &complexDescriptorSet);

        this->pipeline->BindSimpleShaderData(&simpleUniformBuffer,
                                             &textures[0],
                                             *this->descriptorPool,
                                             &simpleDescriptorSet);
    }

    void Entity::DestroyEntity() { }

    void Entity::RebuildSwapResources(Image & newEnvironmentMap)
    {
        textures->at(3) = newEnvironmentMap;
        DestroyEntity();
        CreateUniformBuffers();
        this->pipeline->BindComplexShaderData(&complexUniformBuffers[0],
                                              textures->data(),
                                              &complexUniformBuffers[1],
                                              *this->descriptorPool,
                                              &complexDescriptorSet);

        this->pipeline->BindSimpleShaderData(&simpleUniformBuffer,
                                             &textures->at(0),
                                             *this->descriptorPool,
                                             &simpleDescriptorSet);
    }

    void Entity::SetShine(float reflectivity, float shineDamper)
    {
        this->reflectivity = reflectivity;
        this->shineDamper  = shineDamper;
    }

    void Entity::SetReflectRefractValues(float refractiveIndexRatio, float reflectRefractFactor)
    {
        this->refractiveIndexRatio = refractiveIndexRatio;
        this->reflectRefractFactor = reflectRefractFactor;
    }

    void Entity::Update(Camera & camera)
    {
        ComplexVertexUniformBufferObject   Vubo {};
        ComplexFragmentUniformBufferObject Fubo {};
        Vubo.model = glm::scale(glm::mat4(1.0f), scale);
        Vubo.model = glm::rotate(Vubo.model, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
        Vubo.model = glm::rotate(Vubo.model, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
        Vubo.model = glm::rotate(Vubo.model, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
        Vubo.model = glm::translate(Vubo.model, location);

        Vubo.view = camera.GetView();
        Vubo.proj = camera.GetProjection();

        Vubo.refractiveIndexRatio = refractiveIndexRatio;

        Fubo.extras.x = this->reflectivity;
        Fubo.extras.y = this->shineDamper;

        Fubo.reflectRefractFactor = reflectRefractFactor;

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

        complexUniformBuffers[0].CopyDataToBuffer(&Vubo, sizeof(Vubo), 0);
        complexUniformBuffers[1].CopyDataToBuffer(&Fubo, sizeof(Fubo), 0);

        SimpleVertexUniformBufferObject SVubo {};

        SVubo.model = Vubo.model;

        SVubo.proj = camera.GetCubeMapProjection();

        for (uint32_t i = 0; i < CUBEMAP_SIDES; i++)
        {
            SVubo.view[i] = camera.GetCubeMapView(i, location);
        }

        simpleUniformBuffer.CopyDataToBuffer(&SVubo, sizeof(SVubo), 0);
    }

    void Entity::CreateUniformBuffers()
    {
        VkDeviceSize ComplexVUboSize = sizeof(ComplexVertexUniformBufferObject);
        VkDeviceSize ComplexFUboSize = sizeof(ComplexFragmentUniformBufferObject);
        complexUniformBuffers[0]     = hostLocalBuffer->GetBuffer(ComplexVUboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        complexUniformBuffers[1]     = hostLocalBuffer->GetBuffer(ComplexFUboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

        VkDeviceSize SimpleVUboSize = sizeof(SimpleVertexUniformBufferObject);

        simpleUniformBuffer = hostLocalBuffer->GetBuffer(SimpleVUboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
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

    VkDescriptorSet & Entity::GetSimpleDescriptorSet() { return simpleDescriptorSet; }
    VkDescriptorSet & Entity::GetComplexDescriptorSet() { return complexDescriptorSet; }
} // namespace sckz