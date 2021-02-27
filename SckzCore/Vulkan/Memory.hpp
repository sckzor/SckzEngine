#pragma once
#include "../Include.hpp"

namespace sckz
{
    struct SubBlock
    {
    public:
        uint32_t         offset;
        VkDeviceMemory * memory;
        uint32_t         size;
        SubBlock_t *     next = nullptr;
    } typedef SubBlock_t;

    struct Block
    {
        VkDeviceMemory * memory;
        SubBlock_t *     beginning;
        uint32_t         remainingSize;
        uint32_t         memoryType;
    } typedef Block_t;

    class Memory
    {
        friend Block_t;
        friend SubBlock_t;

    private:
        std::vector<Block_t> blocks;
        VkDevice *           device;
        VkPhysicalDevice *   physicalDevice;
        uint32_t             blockSize;

    public:
        static uint32_t FindMemoryType(uint32_t                typeFilter,
                                       VkMemoryPropertyFlags & properties,
                                       VkPhysicalDevice &      physicalDevice);

    public:
        void         CreateMemory(VkDevice & device, VkPhysicalDevice & physicalDevice, uint32_t blockSize);
        SubBlock_t & AllocateMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags & properties);
        void         FreeMemory();
        void         Defrag();
        void         DestroyMemory();
    };
} // namespace sckz