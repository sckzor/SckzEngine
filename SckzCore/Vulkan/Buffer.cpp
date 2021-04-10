#include "Buffer.hpp"

namespace sckz
{
    void Buffer::CreateBuffer(VkPhysicalDevice & physicalDevice,
                              VkDevice &         device,
                              Memory &           memory,
                              uint32_t           size,
                              VkBufferUsageFlags usage,
                              VkQueue &          queue)
    {
        this->device    = &device;
        this->blockSize = blockSize;
        this->queue     = &queue;
        this->memory    = &memory;
    }

    Buffer::SubBlock & Buffer::GetBuffer(uint32_t size, VkBufferUsageFlags usage)
    {
        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            if (blocks[i]->remainingSize >= size && blocks[i]->usage == usage)
            {
                SubBlock * subBlock = blocks[i]->beginning;
                while (subBlock->next != nullptr)
                {
                    subBlock = subBlock->next;
                }
                blocks[i]->remainingSize -= size;
                SubBlock * newSubBlock = new SubBlock();

                newSubBlock->size   = size;
                newSubBlock->offset = (subBlock->offset + subBlock->size);
                newSubBlock->next   = nullptr;
                newSubBlock->parent = blocks[i];

                subBlock->next = newSubBlock;
                return *newSubBlock;
            }
        }

        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = this->blockSize;
        bufferInfo.usage       = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        Block * block        = new Block();
        block->beginning     = new SubBlock();
        block->usage         = usage;
        block->remainingSize = blockSize;

        if (vkCreateBuffer(*this->device, &bufferInfo, nullptr, &block->buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*this->device, buffer, &memRequirements);

        memoryBlock = &memory->AllocateMemory(memRequirements, *memoryProperty);
        vkBindBufferMemory(*this->device, buffer, *memoryBlock->memory, memoryBlock->offset);

        blocks.emplace_back(block);

        return GetBuffer(size, usage);
    }

    void Buffer::DestroyBuffer() { vkDestroyBuffer(*this->device, buffer, nullptr); }

    void Buffer::SubBlock::CopyBufferToBuffer(Buffer::SubBlock & buffer, VkCommandPool & pool)
    {
        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(*this->parent->parent->device, pool, *this->parent->parent->queue);

        VkBufferCopy copyRegion {};
        copyRegion.dstOffset = buffer.offset;
        copyRegion.srcOffset = this->offset;
        copyRegion.size      = size;
        vkCmdCopyBuffer(cmdBuffer.GetCommandBuffer(),
                        this->parent->parent->buffer,
                        buffer.parent->parent->buffer,
                        1,
                        &copyRegion);

        cmdBuffer.EndSingleUseCommandBuffer();
    }

    void Buffer::SubBlock::CopyBufferToImage(VkImage & image, VkCommandPool & pool, uint32_t width, uint32_t height)
    {
        CommandBuffer cmdBuffer;
        cmdBuffer.BeginSingleUseCommandBuffer(*this->parent->parent->device, pool, *this->parent->parent->queue);

        VkBufferImageCopy region {};
        region.bufferOffset                    = this->offset;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = { 0, 0, 0 };
        region.imageExtent                     = { width, height, 1 };

        vkCmdCopyBufferToImage(cmdBuffer.GetCommandBuffer(),
                               this->parent->buffer,
                               image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        cmdBuffer.EndSingleUseCommandBuffer();
    }

    void Buffer::SubBlock::DestroySubBlock() {};

    void Buffer::SubBlock::CopyDataToBuffer(void * data, uint32_t dataSize)
    {
        if (dataSize != size)
        {
            throw std::runtime_error("buffer size mis-match!");
        }

        void * location;
        vkMapMemory(*this->parent->parent->device,
                    *this->parent->parent->memoryBlock->memory,
                    this->parent->parent->memoryBlock->offset,
                    size,
                    0,
                    &location);
        memcpy(location + this->offset, data, static_cast<size_t>(size));
        vkUnmapMemory(*this->parent->parent->device, *this->parent->parent->memoryBlock->memory);
    }

    VkBuffer Buffer::GetBuffer() { return buffer; }
} // namespace sckz