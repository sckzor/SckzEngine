#include "Fbo.hpp"

namespace sckz
{
    void Fbo::CreateFBO(VkPhysicalDevice &    physicalDevice,
                        VkDevice &            device,
                        VkRenderPass &        renderPass,
                        Memory &              memory,
                        VkQueue &             graphicsQueue,
                        VkSampleCountFlagBits msaaSamples,
                        VkExtent2D            swapChainExtent)
    {
        this->physicalDevice  = &physicalDevice;
        this->device          = &device;
        this->renderPass      = &renderPass;
        this->memory          = &memory;
        this->graphicsQueue   = &graphicsQueue;
        this->msaaSamples     = msaaSamples;
        this->swapChainExtent = swapChainExtent;

        CreateImage();
        CreateColorResources();
        CreateDepthResources();
        CreateFramebuffer();
    }

    void Fbo::CreateColorResources()
    {
        VkFormat colorFormat = renderedImage.GetFormat();

        colorImage.CreateImage(swapChainExtent.width,
                               swapChainExtent.height,
                               1,
                               msaaSamples,
                               colorFormat,
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               *device,
                               *physicalDevice,
                               *memory,
                               *graphicsQueue);
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
                               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                               *device,
                               *physicalDevice,
                               *memory,
                               *graphicsQueue);
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
        framebufferInfo.renderPass      = *renderPass;
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
                                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  *this->device,
                                  *this->physicalDevice,
                                  *memory,
                                  *this->graphicsQueue);
        renderedImage.CreateTextureSampler();
        renderedImage.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }
} // namespace sckz