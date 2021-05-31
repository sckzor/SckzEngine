#pragma once
#include "../Include.hpp"

namespace sckz
{
    class DescriptorPool
    {
    private:
        uint32_t              allocatedPools = 0;
        static const uint32_t size_inc       = 6;

        std::vector<VkDescriptorPool *> pools;

    private:
        uint32_t   numSwapChainImages;
        VkDevice * device;

    public:
        void               CreateDescriptorPool(VkDevice & device, uint32_t numSwapChainImages);
        void               DestroyDescriptorPool();
        VkDescriptorPool & GetDescriptorPool();

    private:
        void CreatePool();
    };
} // namespace sckz