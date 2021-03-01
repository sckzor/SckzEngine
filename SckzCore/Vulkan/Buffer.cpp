#include "Buffer.hpp"

namespace sckz
{
    void Buffer::CreateBuffer(VkPhysicalDevice &    physicalDevice,
                              VkDevice &            device,
                              Memory &              memory,
                              uint32_t              size,
                              VkBufferUsageFlags    usage,
                              VkMemoryPropertyFlags properties,
                              VkQueue &             queue)
    {
        this->device = &device;
        this->size   = size;
        this->queue  = &queue;

        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = this->size;
        bufferInfo.usage       = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(*this->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*this->device, buffer, &memRequirements);
        /*
        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex
            = Memory::FindMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);

        if (vkAllocateMemory(*this->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        */
        block = &memory.AllocateMemory(memRequirements, properties);
        vkBindBufferMemory(*this->device, buffer, *block->memory, block->offset);
    }

    void Buffer::DestroyBuffer() { vkDestroyBuffer(*this->device, buffer, nullptr); }

    void Buffer::CopyBufferToBuffer(Buffer & buffer, VkCommandPool & pool)
    {
        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(*this->device, pool, *queue);

        VkBufferCopy copyRegion {};
        copyRegion.size = size;
        vkCmdCopyBuffer(cmdBuffer.GetCommandBuffer(), this->buffer, buffer.buffer, 1, &copyRegion);

        cmdBuffer.EndSingleUseCommandBuffer();
    }

    void Buffer::CopyBufferToImage(VkImage & image, VkCommandPool & pool, uint32_t width, uint32_t height)
    {
        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(*this->device, pool, *queue);

        VkBufferImageCopy region {};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = { 0, 0, 0 };
        region.imageExtent                     = { width, height, 1 };

        vkCmdCopyBufferToImage(cmdBuffer.GetCommandBuffer(),
                               buffer,
                               image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        cmdBuffer.EndSingleUseCommandBuffer();
    }

    void Buffer::CopyDataToBuffer(void * data, uint32_t dataSize)
    {
        if (dataSize != size)
        {
            throw std::runtime_error("buffer size mis-match!");
        }

        void * location;
        vkMapMemory(*device, *block->memory, block->offset, size, 0, &location);
        memcpy(location, data, static_cast<size_t>(size));
        vkUnmapMemory(*device, *block->memory);
    }

    VkBuffer Buffer::GetBuffer() { return buffer; }
} // namespace sckz