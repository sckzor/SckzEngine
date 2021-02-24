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

    public:
        void BeginSingleUseCommandBuffer(VkDevice & device, VkCommandPool & pool, VkQueue & queue);

        void EndSingleUseCommandBuffer();
        void BeginCommandBuffer();
        void EndCommandBuffer();

    public:
        VkCommandBuffer & GetCommandBuffer();
    };
} // namespace sckz