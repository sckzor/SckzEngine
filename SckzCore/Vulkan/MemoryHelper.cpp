#include "MemoryHelper.hpp"

namespace sckz
{
    uint32_t MemoryHelper::FindMemoryType(uint32_t                typeFilter,
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
} // namespace sckz