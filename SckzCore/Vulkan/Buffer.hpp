#pragma once
#include "../Include.hpp"
#include "MemoryHelper.hpp"
#include "CommandBuffer.hpp"

namespace sckz {
    class Buffer {
    private:
        VkBuffer        buffer;
        VkDeviceMemory  bufferMemory;
        VkDevice *      device;
        uint32_t        size;
        VkQueue  *      queue;

    public:
        void CreateBuffer       (VkPhysicalDevice &     physicalDevice,
                                 VkDevice         &     device, 
                                 uint32_t               size, 
                                 VkBufferUsageFlags     usage, 
                                 VkMemoryPropertyFlags  properties,
                                 VkQueue          &     queue);

        void CopyBufferToBuffer (Buffer        &        buffer, 
                                 VkCommandPool &        pool);

        void CopyBufferToImage  (VkImage       &        image, 
                                 VkCommandPool &        pool,
                                 uint32_t               width,
                                 uint32_t               height);

        void CopyDataToBuffer   (void          *        data,
                                 uint32_t               dataSize);

        void DestroyBuffer      ();

    public:
        VkBuffer GetBuffer      ();
    };
}