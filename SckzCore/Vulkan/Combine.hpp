#pragma once
#include "../Include.hpp"
#include "CommandBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "Memory.hpp"

namespace sckz
{
    class Combine
    {
    private:
        VkDevice *            device;
        VkPhysicalDevice *    physicalDevice;
        Memory *              memory;
        GraphicsPipeline      filterPipeline;
        VkCommandBuffer       commandBuffer;
        VkExtent2D            swapChainExtent;
        DescriptorPool *      descriptorPool;
        VkCommandPool *       commandPool;
        VkQueue *             graphicsQueue;
        VkFormat              format;
        VkFence               inFlightFence;
        VkSampleCountFlagBits msaaSamples;
        Fbo                   tempFbo;
        Fbo *                 lastRenderedFbo1;
        Fbo *                 lastRenderedFbo2;

    public:
        void CreateCombine(const char *          fragmentFile,
                           const char *          vertexFile,
                           VkDevice &            device,
                           VkPhysicalDevice &    physicalDevice,
                           Memory &              memory,
                           DescriptorPool &      descriptorPool,
                           VkQueue &             graphicsQueue,
                           VkCommandPool &       commandPool,
                           VkFormat              format,
                           VkSampleCountFlagBits msaaSamples,
                           VkExtent2D            swapChainExtent);

        void DestroyCombine();

        Fbo & CombineFbos(Fbo & fbo1, Fbo & fbo2);

        void RebuildSwapResources(DescriptorPool &      descriptorPool,
                                  VkSampleCountFlagBits msaaSamples,
                                  VkExtent2D            swapChainExtent);

    private:
        void CreateCommandBuffer();
        void RebuildCommandBuffer(Fbo * fbo1, Fbo * fbo2);
        void CreateSyncObjects();
    };
} // namespace sckz