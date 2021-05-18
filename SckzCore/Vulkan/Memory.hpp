#pragma once
#include "../Include.hpp"

namespace sckz
{

    class Memory
    {
    public:
        typedef struct SubBlock
        {
        public:
            uint32_t         offset;
            VkDeviceMemory * memory;
            uint32_t         size;
            bool             isFree;
        } SubBlock_t;

        typedef struct Block
        {
        public:
            VkDeviceMemory            memory;
            std::vector<SubBlock_t *> blocks;
            uint32_t                  remainingSize;
            uint32_t                  memoryType;
        } Block_t;

    private:
        std::vector<Block_t *> blocks;
        Memory::SubBlock_t     memoryBlock;
        VkDevice *             device;
        VkPhysicalDevice *     physicalDevice;
        uint32_t               blockSize;

    public:
        static uint32_t FindMemoryType(uint32_t                typeFilter,
                                       VkMemoryPropertyFlags & properties,
                                       VkPhysicalDevice &      physicalDevice);

    public:
        void         CreateMemory(VkDevice & device, VkPhysicalDevice & physicalDevice, uint32_t blockSize);
        SubBlock_t & AllocateMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags & properties);
        void         DeallocateMemory(SubBlock_t & block);
        void         FreeMemory();
        void         Defrag();
        void         DestroyMemory();
    };
} // namespace sckz