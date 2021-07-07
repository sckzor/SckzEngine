#pragma once
#include "../../Include.hpp"
#include "CommandBuffer.hpp"
#include "Memory.hpp"

namespace sckz
{
    class Buffer
    {
    public:
        class SubBlock;
        // Block Struct
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
            uint32_t   targetSize;
            SubBlock * next = nullptr;
            bool       isFree;

            SubBlock() = default;

            void CopyBufferToBuffer(Buffer::SubBlock & buffer);

            void CopyBufferToImage(VkImage & image, uint32_t width, uint32_t height, bool isCube);

            void CopyDataToBuffer(void * data, uint32_t dataSize, uint32_t offset);

            void DestroySubBlock();
        };

    private:
        std::vector<Block *>  blocks;
        Memory::SubBlock_t *  memoryBlock;
        VkDevice *            device;
        uint32_t              blockSize;
        VkQueue *             queue;
        Memory *              memory;
        VkCommandPool *       pool;
        VkMemoryPropertyFlags memoryProperty;
        CommandBuffer         cmdBuffer;

    public:
        void CreateBuffer(VkPhysicalDevice &    physicalDevice,
                          VkDevice &            device,
                          Memory &              memory,
                          uint32_t              blockSize,
                          VkMemoryPropertyFlags memoryProperty,
                          VkQueue &             queue,
                          VkCommandPool &       pool);

        SubBlock & GetBuffer(uint32_t size, VkBufferUsageFlags usage);
        // Stuff
        void DestroyBuffer();
    };
} // namespace sckz