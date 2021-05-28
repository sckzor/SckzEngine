#include "GraphicsPipeline.hpp"

namespace sckz
{
    void GraphicsPipeline::CreatePipeline(VkDevice &            device,
                                          VkExtent2D            extent,
                                          VkRenderPass &        renderPass,
                                          VkSampleCountFlagBits msaaSamples,
                                          const char *          vertexFile,
                                          const char *          fragmentFile,
                                          PipelineType          type)
    {
        this->device       = &device;
        this->extent       = extent;
        this->renderPass   = &renderPass;
        this->msaaSamples  = msaaSamples;
        this->vertexFile   = vertexFile;
        this->fragmentFile = fragmentFile;
        this->type         = type;

        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();
    }

    void GraphicsPipeline::CreatePipeline(VkDevice &            device,
                                          VkExtent2D            extent,
                                          VkRenderPass &        renderPass,
                                          VkSampleCountFlagBits msaaSamples)
    {
        this->device      = &device;
        this->extent      = extent;
        this->renderPass  = &renderPass;
        this->msaaSamples = msaaSamples;
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
        if (this->type == PipelineType::FBO_PIPELINE)
        {
            VkDescriptorSetLayoutBinding samplerLayoutBinding {};
            samplerLayoutBinding.binding            = 0;
            samplerLayoutBinding.descriptorCount    = 1;
            samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo {};

            std::array<VkDescriptorSetLayoutBinding, 1> bindings = { samplerLayoutBinding };

            layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings    = bindings.data();

            if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
        }
        else
        {
            VkDescriptorSetLayoutBinding vertUboLayoutBinding {};
            vertUboLayoutBinding.binding            = 0;
            vertUboLayoutBinding.descriptorCount    = 1;
            vertUboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            vertUboLayoutBinding.pImmutableSamplers = nullptr;
            vertUboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;

            VkDescriptorSetLayoutBinding samplerLayoutBinding {};
            samplerLayoutBinding.binding            = 1;
            samplerLayoutBinding.descriptorCount    = 1;
            samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutBinding fragUboLayoutBinding {};
            fragUboLayoutBinding.binding            = 2;
            fragUboLayoutBinding.descriptorCount    = 1;
            fragUboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            fragUboLayoutBinding.pImmutableSamplers = nullptr;
            fragUboLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo {};

            std::array<VkDescriptorSetLayoutBinding, 3> bindings
                = { vertUboLayoutBinding, samplerLayoutBinding, fragUboLayoutBinding };

            layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings    = bindings.data();

            if (vkCreateDescriptorSetLayout(*device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create descriptor set layout!");
            }
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
        viewport.width    = (float)extent.width;
        viewport.height   = (float)extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor {};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

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
        multisampling.rasterizationSamples = msaaSamples; // TODO: Make this extensable

        VkPipelineDepthStencilStateCreateInfo depthStencil {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        if (type == PipelineType::GUI_PIPELINE)
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
            colorBlendAttachment.blendEnable = VK_TRUE;
        }
        else
        {
            colorBlendAttachment.blendEnable = VK_FALSE;
        }
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

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
        pipelineInfo.renderPass          = *renderPass;
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

    void GraphicsPipeline::BindImage(Image & texture, DescriptorPool & pool, VkDescriptorSet * descriptorSet)
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

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView   = texture.GetImageView();
        imageInfo.sampler     = texture.GetSampler();

        std::array<VkWriteDescriptorSet, 1> descriptorWrites {};

        descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet          = *descriptorSet;
        descriptorWrites[0].dstBinding      = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo      = &imageInfo;

        vkUpdateDescriptorSets(*device,
                               static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(),
                               0,
                               nullptr);
    }
} // namespace sckz