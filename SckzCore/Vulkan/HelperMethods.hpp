#include "../Include.hpp"

namespace sckz
{
    class HelperMethods
    {
    public:
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
        };

    private:
    public:
        static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice & device, VkSurfaceKHR & surface);
        static VkFormat           FindDepthFormat(VkPhysicalDevice & physicalDevice);
        static VkFormat           FindSupportedFormat(const std::vector<VkFormat> & candidates,
                                                      VkImageTiling                 tiling,
                                                      VkFormatFeatureFlags          features,
                                                      VkPhysicalDevice &            physicalDevice);
    };
} // namespace sckz