#pragma once
#include "../Include.hpp"

namespace sckz
{
    class MemoryHelper
    {
    public:
        static uint32_t
        FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags & properties, VkPhysicalDevice & physicalDevice);
    };
} // namespace sckz