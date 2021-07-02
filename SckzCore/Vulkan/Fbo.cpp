#include "Fbo.hpp"

namespace sckz
{
    void Fbo::CreateFBO(VkPhysicalDevice &    physicalDevice,
                        VkDevice &            device,
                        Memory &              memory,
                        VkQueue &             graphicsQueue,
                        VkFormat              format,
                        VkSampleCountFlagBits msaaSamples,
                        VkExtent2D            swapChainExtent,
                        VkCommandPool &       pool)
    {
        this->physicalDevice  = &physicalDevice;
        this->device          = &device;
        this->memory          = &memory;
        this->format          = format;
        this->graphicsQueue   = &graphicsQueue;
        this->msaaSamples     = msaaSamples;
        this->swapChainExtent = swapChainExtent;
        this->pool            = &pool;

        CreateImage();
        CreateRenderPass();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffer();
    }

    void Fbo::DestroySwapResources()
    {

        vkDestroyFramebuffer(*device, renderedImageFrameBuffer, nullptr);
        vkDestroyRenderPass(*device, renderPass, nullptr);

        colorImage.DestroyImage();
        depthImage.DestroyImage();

        renderedImage.DestroyImage();
    }

    void Fbo::CreateDummyFBO(VkRenderPass & renderPass, VkSampleCountFlagBits msaaSamples, VkExtent2D swapChainExtent)
    {
        this->renderPass      = renderPass;
        this->msaaSamples     = msaaSamples;
        this->swapChainExtent = swapChainExtent;
    }

    void Fbo::DestroyFBO() { DestroySwapResources(); }

    void Fbo::CreateColorResources()
    {
        VkFormat colorFormat = renderedImage.GetFormat();

        colorImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               msaaSamples,
                               colorFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                   | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               *device,
                               *physicalDevice,
                               *memory,
                               *graphicsQueue,
                               *pool);
        colorImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Fbo::CreateDepthResources()
    {
        VkFormat depthFormat = HelperMethods::FindDepthFormat(*physicalDevice);

        depthImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               msaaSamples,
                               depthFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                   | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               *device,
                               *physicalDevice,
                               *memory,
                               *graphicsQueue,
                               *pool);
        depthImage.CreateImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void Fbo::CreateFramebuffer()
    {
        std::vector<VkImageView> attachments;

        if (msaaSamples == VK_SAMPLE_COUNT_1_BIT)
        {
            attachments.push_back(renderedImage.GetImageView());
            attachments.push_back(depthImage.GetImageView());
        }
        else
        {
            attachments.push_back(colorImage.GetImageView());
            attachments.push_back(depthImage.GetImageView());
            attachments.push_back(renderedImage.GetImageView());
        }

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments    = attachments.data();
        framebufferInfo.width           = swapChainExtent.width;
        framebufferInfo.height          = swapChainExtent.height;
        framebufferInfo.layers          = 1;

        if (vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &renderedImageFrameBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    void Fbo::CreateImage()
    {
        renderedImage.CreateImage(swapChainExtent.width,
                                  swapChainExtent.height,
                                  1,
                                  VK_SAMPLE_COUNT_1_BIT,
                                  this->format,
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                                      | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
                                      | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  *this->device,
                                  *this->physicalDevice,
                                  *memory,
                                  *this->graphicsQueue,
                                  *pool);
        renderedImage.CreateTextureSampler();
        renderedImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Fbo::RebuildSwapResources(VkSampleCountFlagBits msaaSamples, VkExtent2D swapChainExtent)
    {
        this->msaaSamples     = msaaSamples;
        this->swapChainExtent = swapChainExtent;
        DestroySwapResources();

        CreateImage();
        CreateRenderPass();
        CreateDepthResources();
        CreateColorResources();
        CreateFramebuffer();
    }

    void Fbo::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment {};
        colorAttachment.format         = renderedImage.GetFormat();
        colorAttachment.samples        = msaaSamples;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        if (msaaSamples == VK_SAMPLE_COUNT_1_BIT)
        {
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        else
        {
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        VkAttachmentDescription depthAttachment {};
        depthAttachment.format         = HelperMethods::FindDepthFormat(*physicalDevice);
        depthAttachment.samples        = msaaSamples;
        depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve {};
        colorAttachmentResolve.format         = renderedImage.GetFormat();
        colorAttachmentResolve.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef {};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        if (msaaSamples != VK_SAMPLE_COUNT_1_BIT)
        {
            subpass.pResolveAttachments = &colorAttachmentResolveRef;
        }

        VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask
            = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask
            = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };

        if (msaaSamples != VK_SAMPLE_COUNT_1_BIT)
        {
            attachments.push_back(colorAttachmentResolve);
        }

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments    = attachments.data();
        renderPassInfo.subpassCount    = 1;

        renderPassInfo.pSubpasses = &subpass;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        if (vkCreateRenderPass(*device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void Fbo::CopyToFbo(Fbo & dst)
    {
        colorImage.CopyImage(dst.colorImage, VK_IMAGE_ASPECT_COLOR_BIT);
        depthImage.CopyImage(dst.depthImage, VK_IMAGE_ASPECT_DEPTH_BIT);
        renderedImage.CopyImage(dst.renderedImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Fbo::GetRenderPassBeginInfo(VkRenderPassBeginInfo * renderPassInfo)
    {
        renderPassInfo->sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo->renderPass        = renderPass;
        renderPassInfo->framebuffer       = renderedImageFrameBuffer;
        renderPassInfo->renderArea.offset = { 0, 0 };
        renderPassInfo->renderArea.extent = swapChainExtent;
    }

    Image &        Fbo::GetImage() { return renderedImage; }
    VkRenderPass & Fbo::GetRenderPass() { return renderPass; }
    VkExtent2D     Fbo::GetSwapChainExtent() { return swapChainExtent; }

    VkSampleCountFlagBits Fbo::GetMSAASamples() { return msaaSamples; }

    VkFramebuffer & Fbo::GetImageFramebuffer() { return renderedImageFrameBuffer; }
} // namespace sckz