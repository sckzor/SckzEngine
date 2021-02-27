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

    SubBlock_t & Memory::AllocateMemory(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags & properties)
    {
        uint32_t memoryType = FindMemoryType(memoryRequirements.memoryTypeBits, properties, *physicalDevice);
        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            if (blocks[i].remainingSize >= memoryRequirements.size && memoryType == blocks[i].memoryType)
            {
                SubBlock_t * subBlock = blocks[i].beginning;
                while (subBlock->next != nullptr)
                {
                    subBlock = subBlock->next;
                }
                blocks[i].remainingSize -= memoryRequirements.size;
                SubBlock_t * newSubBlock = new SubBlock_t;

                newSubBlock->size   = memoryRequirements.size;
                newSubBlock->offset = subBlock->offset + subBlock->size;
                newSubBlock->next   = nullptr;
                newSubBlock->memory = blocks[i].memory;

                subBlock->next = newSubBlock;
                return *newSubBlock;
            }
        }

        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryType;

        Block_t * block      = new Block_t;
        block->beginning     = new SubBlock_t;
        block->memoryType    = memoryType;
        block->remainingSize = blockSize;
        vkAllocateMemory(*this->device, &allocInfo, nullptr, block->memory);

        blocks.emplace_back(block);

        return AllocateMemory(memoryRequirements, properties);
    }

    void Memory::FreeMemory()
    {
        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            vkFreeMemory(*device, *blocks[i].memory, nullptr);
        }
    }
} // namespace sckz