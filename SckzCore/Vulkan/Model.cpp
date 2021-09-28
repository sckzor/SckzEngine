#include "Model.hpp"

#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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

        this->hostLocalBuffer.CreateBuffer(device,
                                           memory,
                                           0x7FFFFFF,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           queue,
                                           commandPool);

        this->deviceLocalBuffer
            .CreateBuffer(device, memory, 0x7FFFFFF, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, queue, commandPool);

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

        blankTexture.DestroyImage();
        blankTextureCube.DestroyImage();

        hostLocalBuffer.DestroyBuffer();
        deviceLocalBuffer.DestroyBuffer();
        for (uint32_t i = 0; i < textures.size(); i++)
        {
            if (textures[i].GetImage() != blankTexture.GetImage())
            {

                textures[i].DestroyImage();
            }
        }
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
        const aiScene * scene = aiImportFile(modelFileName, aiProcessPreset_TargetRealtime_MaxQuality);

        if (!scene)
        {
            std::cerr << "Could not load file " << modelFileName << std::endl;
            return;
        }

        // Load all meshes and whatnot
        aiMesh * mesh;
        uint32_t maxVertices = 0;

        // this is a hack, it chooese the mesh with the most vertices because assimp is being completely idiotic and
        // splitting one mesh into multiple for reasons that are completely beyond me...

        for (uint32_t i = 0; i < scene->mNumMeshes; i++)
        {
            if (scene->mMeshes[i]->mNumVertices > maxVertices)
            {
                maxVertices = scene->mMeshes[i]->mNumVertices;
                mesh        = scene->mMeshes[i];
            }
        }

        for (std::uint32_t vertIdx = 0u; vertIdx < mesh->mNumVertices; vertIdx++)
        {
            aiVector3D vert = mesh->mVertices[vertIdx];
            aiVector3D norm = mesh->mNormals[vertIdx];
            aiVector3D uv;
            if (mesh->mTextureCoords[0] != nullptr)
            {
                uv = mesh->mTextureCoords[0][vertIdx];
            }
            else
            {
                uv.x = 0;
                uv.y = 0;
            }

            vertices.push_back(Vertex { glm::vec3(vert.x, vert.y, vert.z),
                                        glm::vec3(norm.x, norm.y, norm.z),
                                        glm::vec2(uv.x, 1.0f - uv.y),
                                        glm::ivec4(-1, -1, -1, -1),
                                        glm::vec4(0, 0, 0, 0) });
        }
        ExtractBoneWeightForVertices(mesh, scene);

        for (std::uint32_t faceIdx = 0u; faceIdx < mesh->mNumFaces; faceIdx++)
        {
            indices.push_back(mesh->mFaces[faceIdx].mIndices[0u]);
            indices.push_back(mesh->mFaces[faceIdx].mIndices[1u]);
            indices.push_back(mesh->mFaces[faceIdx].mIndices[2u]);
        }
    }

    void Model::ExtractBoneWeightForVertices(aiMesh * mesh, const aiScene * scene)
    {
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int         boneID   = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneMap.find(boneName) == boneMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id     = boneCount;
                newBoneInfo.offset = BoneInfo::Mat4x4FromAssimp(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneMap[boneName]  = newBoneInfo;
                boneID             = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights    = mesh->mBones[boneIndex]->mWeights;
            int  numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int   vertexId = weights[weightIndex].mVertexId;
                float weight   = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());

                for (int i = 0; i < BoneInfo::MAX_BONES_PER_VERTEX; ++i)
                {
                    if (vertices[vertexId].boneIds[i] < 0)
                    {
                        vertices[vertexId].weights[i] = weight;
                        vertices[vertexId].boneIds[i] = boneID;
                        break;
                    }
                }
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
        this->descriptorPool = &descriptorPool;

        for (size_t i = 0; i < entities.size(); i++)
        {
            entities[i]->RebuildSwapResources(swapChainExtent);
        }
        // Create

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

    Entity & Model::CreateEntity(bool isReflectRefractive, bool isAnimated)
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
                             isAnimated,
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

    std::map<std::string, BoneInfo> & Model::GetBoneMap() { return boneMap; }
    uint32_t                          Model::GetBoneCount() { return boneCount; }

    void Model::DestroySwapResources()
    {
        vkFreeCommandBuffers(*device, *commandPool, 1, &commandBuffer);
        vkFreeCommandBuffers(*device, *commandPool, 1, &cubeMapCommandBuffer);
    }
} // namespace sckz