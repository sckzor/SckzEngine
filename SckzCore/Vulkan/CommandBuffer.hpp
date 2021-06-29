#pragma once
#include "../Include.hpp"

namespace sckz
{
    class CommandBuffer
    {
    private:
        VkCommandBuffer commandBuffer;
        VkDevice *      device;
        VkCommandPool * pool;
        VkQueue *       queue;
        VkFence         inFlightFence;

    public:
        void BeginSingleUseCommandBuffer(VkDevice & device, VkCommandPool & pool, VkQueue & queue);

        void EndSingleUseCommandBuffer();

    public:
        VkCommandBuffer & GetCommandBuffer();
    };
} // namespace sckz