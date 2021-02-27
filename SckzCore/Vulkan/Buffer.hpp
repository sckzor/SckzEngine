#pragma once
#include "../Include.hpp"
#include "CommandBuffer.hpp"
#include "Memory.hpp"

namespace sckz
{
    class Buffer
    {
    private:
        VkBuffer     buffer;
        SubBlock_t * block;
        VkDevice *   device;
        uint32_t     size;
        VkQueue *    queue;

    public:
        void CreateBuffer(VkPhysicalDevice &    physicalDevice,
                          VkDevice &            device,
                          Memory &              memory,
                          uint32_t              size,
                          VkBufferUsageFlags    usage,
                          VkMemoryPropertyFlags properties,
                          VkQueue &             queue);

        void CopyBufferToBuffer(Buffer & buffer, VkCommandPool & pool);

        void CopyBufferToImage(VkImage & image, VkCommandPool & pool, uint32_t width, uint32_t height);

        void CopyDataToBuffer(void * data, uint32_t dataSize);

        void DestroyBuffer();

    public:
        VkBuffer GetBuffer();
    };
} // namespace sckz