#include "DescriptorPool.hpp"

namespace sckz {
    void DescriptorPool::CreateDescriptorPool(VkDevice & device, uint32_t numSwapChainImages) {
        this->device = & device;
        this->numSwapChainImages = numSwapChainImages;
        CreatePool();
    }

    VkDescriptorPool & DescriptorPool::GetDescriptorPool() {
        if(allocatedPools < size_inc) {
            allocatedPools += 1;
            return * pools.back();
        } else {
            CreatePool();
            allocatedPools = 0;
            return * pools.back();
        }
    }

    void DescriptorPool::CreatePool() {
        pools.push_back(new VkDescriptorPool());
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(numSwapChainImages * size_inc);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(numSwapChainImages * size_inc);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(numSwapChainImages * size_inc);

        if (vkCreateDescriptorPool(* device, & poolInfo, nullptr, pools.back()) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void DescriptorPool::DestroyDescriptorPool() {
        for(uint32_t i = 0; i < pools.size(); i++) {
            vkDestroyDescriptorPool(* device, * pools[i], nullptr);
        }

        pools.clear();
    }
}