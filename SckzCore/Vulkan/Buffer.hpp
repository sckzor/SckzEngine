#pragma once
#include "../Include.hpp"
#include "CommandBuffer.hpp"
#include "Memory.hpp"

namespace sckz
{
    class Buffer
    {
    public:
        class SubBlock;

        class Block
        {
        public:
            VkBuffer           buffer;
            SubBlock *         beginning;
            uint32_t           remainingSize;
            VkBufferUsageFlags usage;
            Buffer *           parent;
        };

        class SubBlock
        {
        public:
            uint32_t   offset;
            Block *    parent;
            uint32_t   size;
            SubBlock * next = nullptr;
            bool       isFree;

            SubBlock() = default;

            void CopyBufferToBuffer(Buffer::SubBlock & buffer, VkCommandPool & pool);

            void CopyBufferToImage(VkImage & image, VkCommandPool & pool, uint32_t width, uint32_t height);

            void CopyDataToBuffer(void * data, uint32_t dataSize);

            void DestroySubBlock();
        };

    private:
        std::vector<Block *>  blocks;
        Memory::SubBlock_t *  memoryBlock;
        VkDevice *            device;
        uint32_t              blockSize;
        VkQueue *             queue;
        Memory *              memory;
        VkMemoryPropertyFlags memoryProperty;

    public:
        void CreateBuffer(VkPhysicalDevice &    physicalDevice,
                          VkDevice &            device,
                          Memory &              memory,
                          uint32_t              blockSize,
                          VkMemoryPropertyFlags memoryProperty,
                          VkQueue &             queue);

        SubBlock & GetBuffer(uint32_t size, VkBufferUsageFlags usage);

        void DestroyBuffer();
    };
} // namespace sckz