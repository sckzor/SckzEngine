#include "Gui.hpp"

namespace sckz
{
    void Gui::CreateGUI(const char *       textureFileName,
                        VkCommandPool &    commandPool,
                        VkRenderPass &     renderPass,
                        VkDevice &         device,
                        VkPhysicalDevice & physicalDevice,
                        VkFramebuffer &    framebuffer,
                        GraphicsPipeline * pipeline,
                        DescriptorPool &   descriptorPool,
                        VkExtent2D         swapChainExtent,
                        Memory &           memory,
                        VkQueue &          queue)
    {
        this->textureFileName = textureFileName;
        this->renderPass      = &renderPass;
        this->device          = &device;
        this->physicalDevice  = &physicalDevice;
        this->commandPool     = &commandPool;
        this->descriptorPool  = &descriptorPool;
        this->pipeline        = pipeline;
        this->framebuffer     = &framebuffer;
        this->swapChainExtent = swapChainExtent;
        this->queue           = &queue;
        this->memory          = &memory;

        this->hostLocalBuffer.CreateBuffer(*this->physicalDevice,
                                           *this->device,
                                           *this->memory,
                                           0x7FFFFFF,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           *this->queue);

        texture.CreateTextureImage(textureFileName, *this->device, *this->physicalDevice, memory, commandPool, queue);

        texture.CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
        texture.CreateTextureSampler();
    }
} // namespace sckz