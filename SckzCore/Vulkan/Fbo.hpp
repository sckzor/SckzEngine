#pragma once
#include "../Include.hpp"
#include "DescriptorPool.hpp"
#include "HelperMethods.hpp"
#include "Image.hpp"

namespace sckz
{
    class Fbo
    {
    public:
        VkPhysicalDevice * physicalDevice;
        VkDevice *         device; // Stays

        Image         colorImage;
        Image         depthImage;
        Image         renderedImage;
        VkFramebuffer renderedImageFrameBuffer;

        VkRenderPass * renderPass;      // Duplicated
        VkExtent2D     swapChainExtent; // Stays

        DescriptorPool descriptorPool; // Duplicated
        VkQueue *      graphicsQueue;

        VkFormat format;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // Goes

        Memory * memory;

    public:
        void CreateFBO(VkPhysicalDevice &    physicalDevice,
                       VkDevice &            device,
                       VkRenderPass &        renderPass,
                       Memory &              memory,
                       VkQueue &             graphicsQueue,
                       VkSampleCountFlagBits msaaSamples,
                       VkExtent2D            swapChainExtent);

        void DestroyFBO();

    private:
        void CreateColorResources();
        void CreateDepthResources();

        void CreateImage();

        void CreateFramebuffer();

    public:
        Image & GetRenderedImage();
    };
} // namespace sckz