#pragma once
#include "../../Include.hpp"
#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Memory.hpp"

namespace sckz
{
    class Image
    {
    private:
        VkImage              image;
        VkImageView          imageView;
        VkSampler            sampler;
        Memory::SubBlock_t * block;
        Memory *             memory;
        Buffer               hostLocalBuffer;
        Buffer               deviceLocalBuffer;
        VkFramebuffer        imageFrameBuffer;
        VkDevice *           device;
        VkPhysicalDevice *   physicalDevice;
        VkFormat             format;
        VkQueue *            queue;
        bool                 holdsRealImage;
        bool                 isCube = false;
        uint32_t             mipLevels;
        VkImageLayout        imageLayout;
        VkExtent2D           size;
        VkCommandPool *      pool;
        VkCommandBuffer      copyCmdBuffer;
        VkFence              imageInFlight;

    public:
        void CreateImage(uint32_t              width,
                         uint32_t              height,
                         uint32_t              mipLevels,
                         VkSampleCountFlagBits numSamples,
                         VkFormat              format,
                         VkImageTiling         tiling,
                         VkImageUsageFlags     usage,
                         VkMemoryPropertyFlags properties,
                         VkDevice &            device,
                         VkPhysicalDevice &    physicalDevice,
                         Memory &              memory,
                         VkQueue &             queue,
                         VkCommandPool &       pool,
                         bool                  isCube);

        void CreateImage(VkDevice & device,
                         VkImage &  image,
                         VkFormat & imageFormat,
                         uint32_t   mipLevels,
                         Memory &   memory,
                         VkExtent2D size);

        void DestroyImage();

        void CreateImageView(VkImageAspectFlags aspectFlags);

        void CreateTextureImage(const char *       fileName,
                                VkDevice &         device,
                                VkPhysicalDevice & physicalDevice,
                                Memory &           memory,
                                VkCommandPool &    pool,
                                VkQueue &          queue);

        void CreateCubeTextureImage(const char *       fileNameFront,
                                    const char *       fileNameBack,
                                    const char *       fileNameUp,
                                    const char *       fileNameDown,
                                    const char *       fileNameRight,
                                    const char *       fileNameLeft,
                                    VkDevice &         device,
                                    VkPhysicalDevice & physicalDevice,
                                    Memory &           memory,
                                    VkCommandPool &    pool,
                                    VkQueue &          queue);

        void CreateBlankTextureImage(VkDevice &         device,
                                     VkPhysicalDevice & physicalDevice,
                                     Memory &           memory,
                                     VkCommandPool &    pool,
                                     VkQueue &          queue);

        void CopyImage(Image & dst, VkImageAspectFlagBits aspectMask);
        void GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight);

        void TransitionImageLayout(VkImageLayout         oldLayout,
                                   VkImageLayout         newLayout,
                                   VkImageAspectFlagBits aspectMask,
                                   VkCommandBuffer &     cmdBuffer);

        VkSampleCountFlagBits GetMaxUsableSampleCount();

        void CreateTextureSampler();

    private:
        void CreateCommandBuffer();
        void DestroyCommandBuffer();
        void CreateSyncObjects();
        void DestroySyncObjects();

        void BeginCommandBuffer();
        void EndCommandBuffer();

    public:
        VkImage &     GetImage();
        VkImageView & GetImageView();
        VkSampler &   GetSampler();
        VkFormat      GetFormat();

        bool operator!=(Image & other);
    };
} // namespace sckz