#pragma once
#include "../Include.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "Vertex.hpp"
#include "Pipeline.hpp"

namespace sckz {
    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
    class Model {
    private:
        Image                          texture;

        std::vector<Vertex>            vertices;
        std::vector<uint32_t>          indices;

        Buffer                         vertexBuffer;
        Buffer                         indexBuffer;
        std::vector<Buffer>            uniformBuffers;

        std::vector<VkDescriptorSet>   descriptorSets;
        std::vector<VkCommandBuffer>   commandBuffers;

        VkDevice                     * device;
        VkPhysicalDevice             * physicalDevice;
        VkCommandPool                * commandPool;
        VkDescriptorPool             * descriptorPool;
        Pipeline                     * pipeline;
        std::vector<VkFramebuffer>   * swapChainFramebuffers;
        VkExtent2D                     swapChainExtent;
        uint32_t                       numSwapChainImages;
        VkQueue                      * queue;

        const char                   * textureFileName;
        const char                   * modelFileName;
    
    public:
        void CreateModel            (const char                 * textureFileName, 
                                     const char                 * modelFileName, 
                                     VkCommandPool              & commandPool, 
                                     VkDevice                   & device, 
                                     VkPhysicalDevice           & physicalDevice, 
                                     std::vector<VkFramebuffer> & swapChainFramebuffers,
                                     Pipeline                   & pipeline,
                                     VkDescriptorPool           & descriptorPool,
                                     VkExtent2D                   swapChainExtent,
                                     uint32_t                     numSwapChainImages,
                                     VkQueue                    & queue);
        void DestroyModel           ();

        void Update                 (uint32_t                     currentImage);

        void DestroySwapResources   ();
        void CreateSwapResources    (Pipeline                   & pipeline,
                                     VkDescriptorPool           & descriptorPool,
                                     VkExtent2D                   swapChainExtent);

    public:
        void LoadModel              ();
        void CreateVertexBuffer     ();
        void CreateIndexBuffer      ();
        void CreateUniformBuffers   ();

        void CreateDescriptorSets   ();
        void CreateCommandBuffers   ();

    public:
        std::vector<VkCommandBuffer> & GetCommandBuffers ();
        Buffer                         GetIndexBuffer    ();
        Buffer                         GetVertexBuffer   ();
        std::vector<VkDescriptorSet> & GetDescriptorSets ();
        uint32_t                       GetNumIndices     ();
    };
}