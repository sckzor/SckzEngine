#include "Buffer.hpp"

namespace sckz
{
    void Buffer::CreateBuffer(VkPhysicalDevice &    physicalDevice,
                              VkDevice &            device,
                              Memory &              memory,
                              uint32_t              blockSize,
                              VkMemoryPropertyFlags memoryProperty,
                              VkQueue &             queue,
                              VkCommandPool &       pool)
    {
        this->device         = &device;
        this->blockSize      = blockSize;
        this->queue          = &queue;
        this->pool           = &pool;
        this->memory         = &memory;
        this->memoryProperty = memoryProperty;
    }

    Buffer::SubBlock & Buffer::GetBuffer(uint32_t size, VkBufferUsageFlags usage)
    {
        if (size > blockSize)
        {
            throw std::runtime_error("Buffer Size was too big!");
        }

        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            if (blocks[i]->remainingSize >= size && blocks[i]->usage == usage)
            {
                SubBlock * subBlock = blocks[i]->beginning;

                while (subBlock != nullptr && subBlock->next != nullptr)
                {
                    subBlock = subBlock->next;
                }

                blocks[i]->remainingSize -= size;
                SubBlock * newSubBlock = new SubBlock();

                newSubBlock->targetSize = size;
                newSubBlock->size       = size;
                newSubBlock->next       = nullptr;
                newSubBlock->parent     = blocks[i];

                if (subBlock == nullptr)
                {
                    newSubBlock->offset  = 0;
                    blocks[i]->beginning = newSubBlock;
                }
                else
                {
                    newSubBlock->offset
                        = (subBlock->offset + subBlock->size) + (64 - ((subBlock->offset + subBlock->size) % 64));
                    subBlock->next = newSubBlock;
                }

                return *newSubBlock;
            }
        }

        VkBufferCreateInfo bufferInfo {};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = blockSize;
        bufferInfo.usage       = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        Block * block        = new Block();
        block->beginning     = nullptr;
        block->usage         = usage;
        block->remainingSize = blockSize;
        block->parent        = this;

        if (vkCreateBuffer(*this->device, &bufferInfo, nullptr, &block->buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*this->device, block->buffer, &memRequirements);

        memoryBlock = &memory->AllocateMemory(memRequirements, this->memoryProperty);
        vkBindBufferMemory(*this->device, block->buffer, *memoryBlock->memory, memoryBlock->offset);

        blocks.emplace_back(block);

        return GetBuffer(size, usage);
    }

    void Buffer::DestroyBuffer()
    {
        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            vkDestroyBuffer(*device, blocks[i]->buffer, nullptr);
            SubBlock * currentBlock = nullptr;
            SubBlock * nextBlock    = blocks[i]->beginning;

            while (nextBlock != nullptr)
            {
                currentBlock = nextBlock;
                nextBlock    = nextBlock->next;
                delete currentBlock;
            }

            delete blocks[i];
        }
    }

    void Buffer::SubBlock::CopyBufferToBuffer(Buffer::SubBlock & buffer)
    {
        if (this->size != buffer.size)
        {
            throw std::runtime_error("Buffer Size Mis-match!");
        }

        parent->parent->cmdBuffer.BeginSingleUseCommandBuffer(*parent->parent->device,
                                                              *parent->parent->pool,
                                                              *parent->parent->queue);

        VkBufferCopy copyRegion {};
        copyRegion.dstOffset = buffer.offset;
        copyRegion.srcOffset = this->offset;
        copyRegion.size      = this->offset + this->size;
        vkCmdCopyBuffer(parent->parent->cmdBuffer.GetCommandBuffer(),
                        this->parent->buffer,
                        buffer.parent->buffer,
                        1,
                        &copyRegion);

        parent->parent->cmdBuffer.EndSingleUseCommandBuffer();
    }

    void Buffer::SubBlock::CopyBufferToImage(VkImage & image, uint32_t width, uint32_t height, bool isCube)
    {
        parent->parent->cmdBuffer.BeginSingleUseCommandBuffer(*parent->parent->device,
                                                              *parent->parent->pool,
                                                              *parent->parent->queue);

        VkBufferImageCopy region {};
        region.bufferOffset                    = this->offset;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        if (isCube)
        {
            region.imageSubresource.layerCount = 6;
        }
        else
        {
            region.imageSubresource.layerCount = 1;
        }
        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(parent->parent->cmdBuffer.GetCommandBuffer(),
                               this->parent->buffer,
                               image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,
                               &region);

        parent->parent->cmdBuffer.EndSingleUseCommandBuffer();
    }

    void Buffer::SubBlock::DestroySubBlock() {};

    void Buffer::SubBlock::CopyDataToBuffer(void * data, uint32_t dataSize, uint32_t offsetAmount)
    {
        /*
        if (dataSize != this->size)
        {
            throw std::runtime_error("buffer size mis-match!"); // Throw error
        }
        */

        void * location;
        vkMapMemory(*this->parent->parent->device,
                    *this->parent->parent->memoryBlock->memory,
                    this->parent->parent->memoryBlock->offset,
                    this->size,
                    0,
                    &location);

        char * OffsetLocation = (char *)location + this->offset; // stuff
        memcpy(OffsetLocation + offsetAmount, (char *)data, static_cast<size_t>(dataSize));
        vkUnmapMemory(*this->parent->parent->device, *this->parent->parent->memoryBlock->memory);
    }
} // namespace sckz