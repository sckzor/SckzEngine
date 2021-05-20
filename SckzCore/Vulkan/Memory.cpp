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
        auto     it         = blocks.find(memoryType);

        if (it == blocks.end())
        {
            blocks.emplace(memoryType, new std::vector<Block_t *>);
            it = blocks.find(memoryType);
        }

        std::vector<Block_t *> * items = it->second;

        for (uint32_t i = 0; i < items->size(); i++)
        {

            for (uint32_t j = 0; j < items->at(i)->blocks->size(); j++)
            {
                if (items->at(i)->blocks->at(j)->isFree && items->at(i)->blocks->at(j)->size >= memoryRequirements.size)
                {
                    items->at(i)->blocks->at(j)->isFree = false;
                    return *items->at(i)->blocks->at(j);
                }
            }

            if (items->at(i)->remainingSize >= memoryRequirements.size)
            {

                SubBlock_t * newSubBlock = new SubBlock_t();
                if (items->at(i)->blocks->size() > 0)
                {
                    SubBlock_t * lastBlock = items->at(i)->blocks->back();
                    newSubBlock->offset    = (lastBlock->offset + lastBlock->size)
                                        + (memoryRequirements.alignment
                                           - ((lastBlock->offset + lastBlock->size) % memoryRequirements.alignment));
                }
                else
                {
                    newSubBlock->offset = 0;
                }

                items->at(i)->remainingSize -= memoryRequirements.size;

                newSubBlock->size      = memoryRequirements.size;
                newSubBlock->alignment = memoryRequirements.alignment;
                newSubBlock->isFree    = false;

                newSubBlock->memory = &items->at(i)->memory;

                items->at(i)->blocks->push_back(newSubBlock);

                return *newSubBlock;
            }
        }

        Block_t * block = CreateBlock(memoryType);
        // std::cout << "Generating a new block: " << items << std::endl;
        items->emplace_back(block);

        return AllocateMemory(memoryRequirements, properties);
    }

    Memory::Block_t * Memory::CreateBlock(uint32_t type)
    {
        VkMemoryAllocateInfo allocInfo {};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = blockSize;
        allocInfo.memoryTypeIndex = type;

        Block_t * block      = new Block_t();
        block->blocks        = new std::vector<SubBlock_t *>();
        block->remainingSize = blockSize;
        vkAllocateMemory(*this->device, &allocInfo, nullptr, &block->memory);
        return block;
    }

    void Memory::DeallocateMemory(SubBlock_t & block) { block.isFree = true; }

    void Memory::Cleanup()
    {
        for (uint32_t i = 0; i < blocks.size(); i++)
        {
            for (uint32_t j = 0; j < blocks[i]->size(); j++)
            {
            restart:
                for (uint32_t k = 0; k < blocks[i]->at(j)->blocks->size(); k++)
                {
                    if (!blocks[i]->at(i)->blocks->at(k))
                    {
                        goto restart;
                    }
                }
                vkFreeMemory(*device, blocks[i]->at(i)->memory, nullptr);
            }
        }
    }

    std::unordered_map<uint32_t, std::vector<Memory::Block_t *> *> Memory::GetStructure() { return blocks; }

    void Memory::DestroyMemory()
    {
        for (auto & x : blocks)
        {
            for (uint32_t j = 0; j < x.second->size(); j++)
            {
                vkFreeMemory(*device, x.second->at(j)->memory, nullptr);
            }
        }
    }
} // namespace sckz