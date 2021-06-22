#include "GraphicsPipeline.hpp"

namespace sckz
{

    void GraphicsPipeline::CreatePipeline(VkDevice &   device,
                                          Fbo &        fbo,
                                          const char * vertexFile,
                                          const char * fragmentFile,
                                          PipelineType type)
    {
        this->device       = &device;
        this->fbo          = &fbo;
        this->vertexFile   = vertexFile;
        this->fragmentFile = fragmentFile;
        this->type         = type;

        switch (type) // Set the default number of UBOs and Samplers for each different type of pipeline
        {
            case PipelineType::FBO_PIPELINE:
                vertexUboCount   = 0;
                samplerCount     = 1;
                fragmentUboCount = 0;
                break;

            case PipelineType::PARTICLE_PIPELINE:
                vertexUboCount   = 2;
                samplerCount     = 1;
                fragmentUboCount = 0;
                break;

            case PipelineType::GUI_PIPELINE:
                vertexUboCount   = 1;
                samplerCount     = 1;
                fragmentUboCount = 0;
                break;

            case PipelineType::MODEL_PIPELINE:
                vertexUboCount   = 1;
                samplerCount     = 3;
                fragmentUboCount = 1;
                break;

            default:
                vertexUboCount   = 0;
                samplerCount     = 0;
                fragmentUboCount = 0;
                break;
        }

        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();
    }

    void GraphicsPipeline::CreatePipeline(VkDevice & device, Fbo & fbo)
    {
        this->device = &device;
        this->fbo    = &fbo;
        if (vertexFile == nullptr || fragmentFile == nullptr)
        {
            throw std::runtime_error(
                "failed to fragment or vertex file was not deffined, use the other overload of this method first");
        }
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();
    }

    void GraphicsPipeline::DestroyPipeline()
    {
        vkDestroyDescriptorSetLayout(*this->device, this->descriptorSetLayout, nullptr);
        vkDestroyPipeline(*this->device, this->pipeline, nullptr);
        vkDestroyPipelineLayout(*this->device, this->pipelineLayout, nullptr);
    }

    std::vector<char> GraphicsPipeline::ReadFile(const std::string & filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t            fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    VkShaderModule GraphicsPipeline::CreateShaderModule(const std::vector<char> & code)
    {
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode    = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(*device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    void GraphicsPipeline::CreateDescriptorSetLayout()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        uint32_t currentBindingNumber = 0;

        for (uint32_t i = 0; i < vertexUboCount; i++)
        {
            VkDescriptorSetLayoutBinding vertUboLayoutBinding {};
            vertUboLayoutBinding.binding            = currentBindingNumber;
            vertUboLayoutBinding.descriptorCount    = 1;
            vertUboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            vertUboLayoutBinding.pImmutableSamplers = nullptr;
            vertUboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;

            bindings.push_back(vertUboLayoutBinding);

            currentBindingNumber++;
        }

        for (uint32_t i = 0; i < samplerCount; i++)
        {
            VkDescriptorSetLayoutBinding samplerLayoutBinding {};
            samplerLayoutBinding.binding            = currentBindingNumber;
            samplerLayoutBinding.descriptorCount    = 1;
            samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

            bindings.push_back(samplerLayoutBinding);

            currentBindingNumber++;
        }

        for (uint32_t i = 0; i < fragmentUboCount; i++)
        {
            VkDescriptorSetLayoutBinding fragUboLayoutBinding {};
            fragUboLayoutBinding.binding            = currentBindingNumber;
            fragUboLayoutBinding.descriptorCount    = 1;
            fragUboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            fragUboLayoutBinding.pImmutableSamplers = nullptr;
            fragUboLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

            bindings.push_back(fragUboLayoutBinding);

            currentBindingNumber++;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo {};

        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings    = bindings.data();

        if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void GraphicsPipeline::CreateGraphicsPipeline()
    {
        auto vertShaderCode = ReadFile(vertexFile);
        auto fragShaderCode = ReadFile(fragmentFile);

        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
        vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName  = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
        fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName  = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription    = Vertex::GetBindingDescription();
        auto attributeDescriptions = Vertex::GetAttributeDescriptions();

        if (this->type == PipelineType::MODEL_PIPELINE)
        {
            vertexInputInfo.vertexBindingDescriptionCount   = 1;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
            vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();
        }

        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport {};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = (float)fbo->GetSwapChainExtent().width;
        viewport.height   = (float)fbo->GetSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor {};
        scissor.offset = { 0, 0 };
        scissor.extent = fbo->GetSwapChainExtent();

        VkPipelineViewportStateCreateInfo viewportState {};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports    = &viewport;
        viewportState.scissorCount  = 1;
        viewportState.pScissors     = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               = 1.0f;
        if (this->type == PipelineType::FBO_PIPELINE || this->type == PipelineType::GUI_PIPELINE)
        {
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
        }
        else
        {
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        }
        rasterizer.frontFace       = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable  = VK_FALSE;
        multisampling.rasterizationSamples = fbo->GetMSAASamples(); // TODO: Make this extensable

        VkPipelineDepthStencilStateCreateInfo depthStencil {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        if (type == PipelineType::GUI_PIPELINE || type == PipelineType::PARTICLE_PIPELINE)
        {
            depthStencil.depthTestEnable  = VK_FALSE;
            depthStencil.depthWriteEnable = VK_FALSE;
        }
        else
        {
            depthStencil.depthTestEnable  = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
        }
        depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable     = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        colorBlendAttachment.colorWriteMask
            = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        if (type == PipelineType::GUI_PIPELINE)
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
            colorBlendAttachment.blendEnable         = VK_TRUE;
        }
        else if (type == PipelineType::PARTICLE_PIPELINE)
        {
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
            colorBlendAttachment.blendEnable         = VK_TRUE;
        }
        else
        {
            colorBlendAttachment.blendEnable = VK_FALSE;
        }

        VkPipelineColorBlendStateCreateInfo colorBlending {};
        colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable     = VK_FALSE;
        colorBlending.logicOp           = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount   = 1;
        colorBlending.pAttachments      = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts    = &descriptorSetLayout;

        if (vkCreatePipelineLayout(*device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          = 2;
        pipelineInfo.pStages             = shaderStages;
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState   = &multisampling;
        pipelineInfo.pDepthStencilState  = &depthStencil;
        pipelineInfo.pColorBlendState    = &colorBlending;
        pipelineInfo.layout              = pipelineLayout;
        pipelineInfo.renderPass          = fbo->GetRenderPass();
        pipelineInfo.subpass             = 0;
        pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(*device, fragShaderModule, nullptr);
        vkDestroyShaderModule(*device, vertShaderModule, nullptr);
    }

    VkDescriptorSetLayout & GraphicsPipeline::GetDescriptorSetLayout() { return descriptorSetLayout; }

    VkPipeline & GraphicsPipeline::GetPipeline() { return pipeline; }

    VkPipelineLayout & GraphicsPipeline::GetPieplineLayout() { return pipelineLayout; }

    bool GraphicsPipeline::operator==(GraphicsPipeline & otherObject)
    {
        return (this->vertexFile == otherObject.vertexFile && this->fragmentFile == otherObject.fragmentFile);
    }

    void GraphicsPipeline::BindShaderData(Buffer::SubBlock  vUboInfo[],
                                          Image             samplerInfo[],
                                          Buffer::SubBlock  fUboInfo[],
                                          DescriptorPool &  pool,
                                          VkDescriptorSet * descriptorSet)
    {
        VkDescriptorSetLayout       layout(GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = pool.GetDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts        = &layout;

        if (vkAllocateDescriptorSets(*device, &allocInfo, descriptorSet) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        descriptorWrites.resize(vertexUboCount + samplerCount + fragmentUboCount);

        uint32_t currentBindingNumber = 0;

        std::vector<VkDescriptorBufferInfo> VInfo {};
        std::vector<VkDescriptorBufferInfo> FInfo {};
        std::vector<VkDescriptorImageInfo>  imageInfo {};

        VInfo.resize(vertexUboCount);
        FInfo.resize(fragmentUboCount);
        imageInfo.resize(samplerCount);

        for (uint32_t i = 0; i < vertexUboCount; i++)
        {
            if (&vUboInfo[i] == nullptr)
            {
                throw std::runtime_error("[FATAL] Not enough descriptors bound (vertex UBO)");
            }

            VInfo[i].buffer = vUboInfo[i].parent->buffer;
            VInfo[i].offset = vUboInfo[i].offset;
            VInfo[i].range  = vUboInfo[i].targetSize;

            descriptorWrites[currentBindingNumber].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[currentBindingNumber].dstSet          = *descriptorSet;
            descriptorWrites[currentBindingNumber].dstBinding      = currentBindingNumber;
            descriptorWrites[currentBindingNumber].dstArrayElement = 0;
            descriptorWrites[currentBindingNumber].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[currentBindingNumber].descriptorCount = 1;
            descriptorWrites[currentBindingNumber].pBufferInfo     = &VInfo[i];

            currentBindingNumber++;
        }

        for (uint32_t i = 0; i < samplerCount; i++)
        {
            if (&samplerInfo[i] == nullptr)
            {
                throw std::runtime_error("[FATAL] Not enough descriptors bound (sampler)");
            }

            imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo[i].imageView   = samplerInfo[i].GetImageView();
            imageInfo[i].sampler     = samplerInfo[i].GetSampler();

            descriptorWrites[currentBindingNumber].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[currentBindingNumber].dstSet          = *descriptorSet;
            descriptorWrites[currentBindingNumber].dstBinding      = currentBindingNumber;
            descriptorWrites[currentBindingNumber].dstArrayElement = 0;
            descriptorWrites[currentBindingNumber].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[currentBindingNumber].descriptorCount = 1;
            descriptorWrites[currentBindingNumber].pImageInfo      = &imageInfo[i];

            currentBindingNumber++;
        }

        for (uint32_t i = 0; i < fragmentUboCount; i++)
        {
            if (&fUboInfo[i] == nullptr)
            {
                throw std::runtime_error("[FATAL] Not enough descriptors bound (fragment UBO)");
            }

            FInfo[i].buffer = fUboInfo[i].parent->buffer;
            FInfo[i].offset = fUboInfo[i].offset;
            FInfo[i].range  = fUboInfo[i].targetSize;

            descriptorWrites[currentBindingNumber].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[currentBindingNumber].dstSet          = *descriptorSet;
            descriptorWrites[currentBindingNumber].dstBinding      = currentBindingNumber;
            descriptorWrites[currentBindingNumber].dstArrayElement = 0;
            descriptorWrites[currentBindingNumber].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[currentBindingNumber].descriptorCount = 1;
            descriptorWrites[currentBindingNumber].pBufferInfo     = &FInfo[i];

            currentBindingNumber++;
        }

        vkUpdateDescriptorSets(*device,
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0,
                               nullptr);
    }

    Fbo & GraphicsPipeline::GetFbo() { return *fbo; }
} // namespace sckz