#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../Loaders/stb_image.h"

namespace sckz
{
    void Image::CreateImage(uint32_t              width,
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
                            bool                  isCube)
    {
        size.width           = width;
        size.height          = height;
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->format         = format;
        this->queue          = &queue;
        this->mipLevels      = mipLevels;
        this->memory         = &memory;
        this->isCube         = isCube;
        this->pool           = &pool;
        this->sampler        = VK_NULL_HANDLE;
        holdsRealImage       = true;

        CreateSyncObjects();

        VkImageCreateInfo imageInfo {};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = mipLevels;
        if (isCube)
        {
            imageInfo.arrayLayers = 6;                                   // CUBEMAP: set this to 6 for cube map textures
            imageInfo.flags       = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT; // CUBEMAP: add for cube map textures
        }
        else
        {
            imageInfo.arrayLayers = 1;
        }
        imageInfo.format        = format;
        imageInfo.tiling        = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = usage;
        imageInfo.samples       = numSamples; // Make this extensible
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(*this->device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        CreateCommandBuffer();

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(*this->device, image, &memRequirements);

        block = &memory.AllocateMemory(memRequirements, properties);
        vkBindImageMemory(*this->device, image, *block->memory, block->offset);
    }

    void Image::CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = *pool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(*device, &allocInfo, &copyCmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void Image::DestroyCommandBuffer() { vkFreeCommandBuffers(*device, *pool, 1, &copyCmdBuffer); }

    void Image::DestroySyncObjects() { vkDestroyFence(*device, imageInFlight, nullptr); }

    void Image::CreateImage(VkDevice & device,
                            VkImage &  image,
                            VkFormat & format,
                            uint32_t   mipLevels,
                            Memory &   memory,
                            VkExtent2D size)
    {
        this->device    = &device;
        this->image     = image;
        this->format    = format;
        this->memory    = &memory;
        this->size      = size;
        this->mipLevels = mipLevels;
        this->sampler   = VK_NULL_HANDLE;
        holdsRealImage  = false;
    }

    void Image::CreateImageView(VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewInfo {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        if (isCube)
        {
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE; // CUBEMAP: add for cube map textures
        }
        else
        {
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        }
        viewInfo.format                          = format;
        viewInfo.subresourceRange.aspectMask     = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        if (isCube)
        {
            viewInfo.subresourceRange.layerCount = 6;
        }
        else
        {
            viewInfo.subresourceRange.layerCount = 1;
        }

        if (vkCreateImageView(*device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void Image::CreateTextureImage(const char *       fileName,
                                   VkDevice &         device,
                                   VkPhysicalDevice & physicalDevice,
                                   Memory &           memory,
                                   VkCommandPool &    pool,
                                   VkQueue &          queue)
    {
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->memory         = &memory;

        hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                     *this->device,
                                     *this->memory,
                                     0x7FFFFFF,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     queue,
                                     pool);

        deviceLocalBuffer.CreateBuffer(*this->physicalDevice,
                                       *this->device,
                                       *this->memory,
                                       0x7FFFFFF,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                       queue,
                                       pool);

        int          texWidth, texHeight, texChannels;
        stbi_uc *    pixels    = stbi_load(fileName, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        mipLevels              = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        size.width  = texWidth;
        size.height = texHeight;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        Buffer::SubBlock stagingBuffer = hostLocalBuffer.GetBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        stagingBuffer.CopyDataToBuffer(pixels, static_cast<size_t>(imageSize), 0);
        stbi_image_free(pixels);

        CreateImage(texWidth,
                    texHeight,
                    mipLevels,
                    VK_SAMPLE_COUNT_1_BIT,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    *this->device,
                    *this->physicalDevice,
                    memory,
                    queue,
                    pool,
                    false); // queue is the PRIVATE queue

        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(device, pool, queue);
        TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_ASPECT_COLOR_BIT,
                              cmdBuffer.GetCommandBuffer());
        cmdBuffer.EndSingleUseCommandBuffer();

        stagingBuffer.CopyBufferToImage(image,
                                        static_cast<uint32_t>(texWidth),
                                        static_cast<uint32_t>(texHeight),
                                        isCube);
        // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

        stagingBuffer.DestroySubBlock();

        GenerateMipmaps(VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight);
    }

    void Image::CreateCubeTextureImage(const char *       fileNameFront,
                                       const char *       fileNameBack,
                                       const char *       fileNameUp,
                                       const char *       fileNameDown,
                                       const char *       fileNameRight,
                                       const char *       fileNameLeft,
                                       VkDevice &         device,
                                       VkPhysicalDevice & physicalDevice,
                                       Memory &           memory,
                                       VkCommandPool &    pool,
                                       VkQueue &          queue)
    {
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->memory         = &memory;

        hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                     *this->device,
                                     *this->memory,
                                     0x7FFFFFF,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     queue,
                                     pool);

        deviceLocalBuffer.CreateBuffer(*this->physicalDevice,
                                       *this->device,
                                       *this->memory,
                                       0x7FFFFFF,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                       queue,
                                       pool);
        stbi_uc * pixels[6];

        int texWidth, texHeight, texChannels;

        pixels[0] = stbi_load(fileNameFront, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        pixels[1] = stbi_load(fileNameBack, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        pixels[2] = stbi_load(fileNameUp, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        pixels[3] = stbi_load(fileNameDown, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        pixels[4] = stbi_load(fileNameRight, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        pixels[5] = stbi_load(fileNameLeft, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        VkDeviceSize imageSize = texWidth * texHeight * 4 * 6; // CUBEMAP: multiply by 6 for cube map textures
        VkDeviceSize layerSize = imageSize / 6;                // CUBEMAP: add for cube map textures
        mipLevels              = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        size.width  = texWidth;
        size.height = texHeight;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        Buffer::SubBlock stagingBuffer = hostLocalBuffer.GetBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        for (uint32_t i = 0; i < 6; i++)
        {
            stagingBuffer.CopyDataToBuffer(pixels[i], static_cast<size_t>(layerSize), layerSize * i);
            stbi_image_free(pixels[i]);
        }

        CreateImage(texWidth,
                    texHeight,
                    mipLevels,
                    VK_SAMPLE_COUNT_1_BIT,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    *this->device,
                    *this->physicalDevice,
                    memory,
                    queue,
                    pool,
                    true); // queue is the PRIVATE queue

        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(device, pool, queue);
        TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_ASPECT_COLOR_BIT,
                              cmdBuffer.GetCommandBuffer());
        cmdBuffer.EndSingleUseCommandBuffer();

        stagingBuffer.CopyBufferToImage(image,
                                        static_cast<uint32_t>(texWidth),
                                        static_cast<uint32_t>(texHeight),
                                        isCube);
        // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

        stagingBuffer.DestroySubBlock();

        GenerateMipmaps(VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight);
    }

    void Image::CreateBlankTextureImage(VkDevice &         device,
                                        VkPhysicalDevice & physicalDevice,
                                        Memory &           memory,
                                        VkCommandPool &    pool,
                                        VkQueue &          queue)
    {
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->memory         = &memory;
        size.width           = 0;
        size.height          = 0;

        hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                     *this->device,
                                     *this->memory,
                                     0x7FFFFFF,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     queue,
                                     pool);

        deviceLocalBuffer.CreateBuffer(*this->physicalDevice,
                                       *this->device,
                                       *this->memory,
                                       0x7FFFFFF,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                       queue,
                                       pool);

        int          texWidth = 1, texHeight = 1;
        char *       dummyData = new char;
        VkDeviceSize imageSize = 4;
        mipLevels              = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!dummyData)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        Buffer::SubBlock stagingBuffer = hostLocalBuffer.GetBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        stagingBuffer.CopyDataToBuffer(dummyData, static_cast<size_t>(imageSize), 0);
        delete dummyData;

        CreateImage(texWidth,
                    texHeight,
                    mipLevels,
                    VK_SAMPLE_COUNT_1_BIT,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    *this->device,
                    *this->physicalDevice,
                    memory,
                    queue,
                    pool,
                    false); // queue is the PRIVATE queue

        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(device, pool, queue);
        TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_ASPECT_COLOR_BIT,
                              cmdBuffer.GetCommandBuffer());
        cmdBuffer.EndSingleUseCommandBuffer();

        stagingBuffer.CopyBufferToImage(image,
                                        static_cast<uint32_t>(texWidth),
                                        static_cast<uint32_t>(texHeight),
                                        isCube);
        // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

        stagingBuffer.DestroySubBlock();

        GenerateMipmaps(VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight);
    }

    void Image::DestroyImage()
    {
        vkDestroyImageView(*device, imageView, nullptr);
        vkDestroySampler(*device, sampler, nullptr);
        if (holdsRealImage)
        {
            vkDestroyImage(*device, image, nullptr);
        }

        hostLocalBuffer.DestroyBuffer();
        deviceLocalBuffer.DestroyBuffer();

        if (block != nullptr)
        {
            memory->DeallocateMemory(*block);
        }

        if (copyCmdBuffer != nullptr)
        {
            DestroySyncObjects();
            DestroyCommandBuffer();
        }
    }

    void Image::TransitionImageLayout(VkImageLayout         oldLayout,
                                      VkImageLayout         newLayout,
                                      VkImageAspectFlagBits aspectMask,
                                      VkCommandBuffer &     cmdBuffer)
    {
        VkImageMemoryBarrier barrier {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = oldLayout;
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = aspectMask;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        if (isCube)
        {
            barrier.subresourceRange.layerCount = 6; // CUBEMAP: set this to 6 for cube map textures
        }
        else
        {
            barrier.subresourceRange.layerCount = 1;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = 0;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                 && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        imageLayout = newLayout;
    }

    void Image::CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(*device, &fenceInfo, nullptr, &imageInFlight) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    void Image::BeginCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(copyCmdBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }

    void Image::EndCommandBuffer()
    {
        if (vkEndCommandBuffer(copyCmdBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void Image::CopyImage(Image & dst, VkImageAspectFlagBits aspectMask, int32_t faceIndex)
    {
        BeginCommandBuffer();

        TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              aspectMask,
                              copyCmdBuffer);

        dst.TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  aspectMask,
                                  copyCmdBuffer);

        VkOffset3D noOffset { 0, 0, 0 };

        VkImageSubresourceLayers subresourceLayers;
        subresourceLayers.aspectMask = aspectMask;
        if (faceIndex == -1)
        {
            subresourceLayers.baseArrayLayer = 0;
            if (isCube)
            {
                subresourceLayers.layerCount = 6;
            }
            else
            {
                subresourceLayers.layerCount = 1;
            }
        }
        else
        {
            subresourceLayers.baseArrayLayer = faceIndex;
            subresourceLayers.layerCount     = 1;
        }

        subresourceLayers.mipLevel = mipLevels - 1;

        VkImageCopy copyData;
        copyData.dstOffset      = noOffset;
        copyData.dstSubresource = subresourceLayers;
        copyData.srcOffset      = noOffset;
        copyData.srcSubresource = subresourceLayers;
        copyData.extent         = VkExtent3D { size.width, size.height, 1 };

        vkCmdCopyImage(copyCmdBuffer, image, imageLayout, dst.image, dst.imageLayout, 1, &copyData);

        dst.TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                  aspectMask,
                                  copyCmdBuffer);
        EndCommandBuffer();

        vkWaitForFences(*device, 1, &imageInFlight, VK_TRUE, UINT64_MAX);
        vkResetFences(*device, 1, &imageInFlight);

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount     = 0;
        submitInfo.pWaitDstStageMask      = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &copyCmdBuffer;

        vkQueueSubmit(*queue, 1, &submitInfo, imageInFlight);
    }

    void Image::GenerateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight)
    {
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(*physicalDevice, imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(*device, *pool, *queue);

        VkImageMemoryBarrier barrier {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image                           = image;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        if (isCube)
        {
            barrier.subresourceRange.layerCount = 6;
        }
        else
        {
            barrier.subresourceRange.layerCount = 1;
        }
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth  = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout                     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask                 = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(cmdBuffer.GetCommandBuffer(),
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            VkImageBlit blit {};
            blit.srcOffsets[0]                 = { 0, 0, 0 };
            blit.srcOffsets[1]                 = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel       = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            if (isCube)
            {
                blit.srcSubresource.layerCount = 6;
            }
            else
            {
                blit.srcSubresource.layerCount = 1;
            }
            blit.dstOffsets[0]             = { 0, 0, 0 };
            blit.dstOffsets[1]             = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel   = i;
            blit.dstSubresource.baseArrayLayer = 0;
            if (isCube)
            {
                blit.dstSubresource.layerCount = 6;
            }
            else
            {
                blit.dstSubresource.layerCount = 1;
            }

            vkCmdBlitImage(cmdBuffer.GetCommandBuffer(),
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cmdBuffer.GetCommandBuffer(),
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                 0,
                                 0,
                                 nullptr,
                                 0,
                                 nullptr,
                                 1,
                                 &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout                     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout                     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask                 = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask                 = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmdBuffer.GetCommandBuffer(),
                             VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier);

        cmdBuffer.EndSingleUseCommandBuffer();
    }

    VkSampleCountFlagBits Image::GetMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(*physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts
                                  & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    void Image::CreateTextureSampler()
    {
        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(*physicalDevice, &properties);

        VkSamplerCreateInfo samplerInfo {};
        samplerInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        if (isCube)
        {
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        }
        else
        {
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
        samplerInfo.anisotropyEnable        = VK_TRUE;
        samplerInfo.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod                  = 0.0f;
        samplerInfo.maxLod                  = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias              = 0.0f;

        if (vkCreateSampler(*device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
    bool Image::operator!=(Image & other) { return image != other.image; }

    VkImageView & Image::GetImageView() { return imageView; }
    VkImage &     Image::GetImage() { return image; }

    VkSampler & Image::GetSampler() { return sampler; }

    VkFormat Image::GetFormat() { return format; }

} // namespace sckz