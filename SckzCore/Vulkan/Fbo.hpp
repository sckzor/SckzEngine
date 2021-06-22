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

        VkRenderPass renderPass;      // Duplicated
        VkExtent2D   swapChainExtent; // Stays

        DescriptorPool descriptorPool; // Duplicated
        VkQueue *      graphicsQueue;

        VkFormat format;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // Goes

        Memory * memory;

    public:
        void CreateFBO(VkPhysicalDevice &    physicalDevice,
                       VkDevice &            device,
                       Memory &              memory,
                       VkQueue &             graphicsQueue,
                       VkFormat              format,
                       VkSampleCountFlagBits msaaSamples,
                       VkExtent2D            swapChainExtent);

        void CreateDummyFBO(VkRenderPass & renderPass, VkSampleCountFlagBits msaaSamples, VkExtent2D swapChainExtent);
        void RebuildSwapResources(VkSampleCountFlagBits msaaSamples, VkExtent2D swapChainExtent);

        void DestroyFBO();

    private:
        void DestroySwapResources();
        void CreateColorResources();
        void CreateDepthResources();
        void CreateImage();
        void CreateRenderPass();
        void CreateFramebuffer();

    public:
        Image &               GetImage();
        VkFramebuffer &       GetImageFramebuffer();
        VkSampleCountFlagBits GetMSAASamples();
        VkExtent2D            GetSwapChainExtent();
        VkRenderPass &        GetRenderPass();
        void                  GetRenderPassBeginInfo(VkRenderPassBeginInfo & renderPassInfo);
    };
} // namespace sckz