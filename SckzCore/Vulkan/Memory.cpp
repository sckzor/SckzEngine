#include "Memory.hpp"

namespace sckz
{
    void Memory::CreateMemory(VkDevice & device, VkPhysicalDevice & physicalDevice, uint32_t blockSize)
    {
        this->device         = &device;
        this->physicalDevice = &physicalDevice;
        this->blockSize      = blockSize;
    }

    uint32_t Memory::FindMemoryType(uint32_t                typeFilter,
                                    VkMemoryPropertyFlags & properties,
                                    VkPhysicalDevice &      physicalDevice)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {

            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    Memory::SubBlock_t & Memory::AllocateMemory(VkMemoryRequirements    memoryRequirements,
                                                VkMemoryPropertyFlags & properties)
    {
        if (memoryRequirements.size > blockSize)
        {
            throw std::runtime_error("Memory Allocation Size was too big!");
        }
        uint32_t memoryType = FindMemoryType(memoryRequirements.memoryTypeBits, properties, *physicalDevice);
        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            if (memoryType == blocks[i]->memoryType)
            {
                for (int j = 0; j < blocks[i]->blocks.size(); j++)
                {
                    if (blocks[i]->blocks[j]->isFree && blocks[i]->blocks[j]->size >= memoryRequirements.size)
                    {
                        return *blocks[i]->blocks[j];
                    }
                }

                if (blocks[i]->remainingSize >= memoryRequirements.size)
                {
                    SubBlock_t * lastBlock = blocks[i]->blocks.back();
                    blocks[i]->remainingSize -= memoryRequirements.size;
                    SubBlock_t * newSubBlock = new SubBlock_t();

                    newSubBlock->size   = memoryRequirements.size;
                    newSubBlock->offset = (lastBlock->offset + lastBlock->size)
                                        + (memoryRequirements.alignment
                                           - ((lastBlock->offset + lastBlock->size) % memoryRequirements.alignment));
                    newSubBlock->memory = &blocks[i]->memory;

                    blocks[i]->blocks.push_back(newSubBlock);
                    return *newSubBlock;
                }
            }
        }

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = blockSize;
        allocInfo.memoryTypeIndex = memoryType;

        Block_t * block      = new Block_t();
        block->memoryType    = memoryType;
        block->remainingSize = blockSize;
        vkAllocateMemory(*this->device, &allocInfo, nullptr, &block->memory);

        blocks.emplace_back(block);

        return AllocateMemory(memoryRequirements, properties);
    }

    void Memory::DeallocateMemory(SubBlock_t & block) { block.isFree = true; }

    void Memory::FreeMemory()
    {
        for (int i = 0; i < blocks.size(); i++)
        {
            vkFreeMemory(*device, memoryBlock.memory)
        }
    }

    void Memory::DestroyMemory() { }
} // namespace sckz