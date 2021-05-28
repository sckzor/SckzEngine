#pragma once
#include "../Include.hpp"
#include "DescriptorPool.hpp"
#include "Image.hpp"
#include "Vertex.hpp"

namespace sckz
{
    class GraphicsPipeline
    {
    public:
        enum class PipelineType
        {
            FBO_PIPELINE,
            GUI_PIPELINE,
            MODEL_PIPELINE
        };

    private:
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout      pipelineLayout;
        VkPipeline            pipeline;

        VkDevice *            device;
        VkExtent2D            extent;
        VkRenderPass *        renderPass;
        VkSampleCountFlagBits msaaSamples;
        const char *          vertexFile;
        const char *          fragmentFile;
        PipelineType          type;

    private:
        static std::vector<char> ReadFile(const std::string & filename);
        VkShaderModule           CreateShaderModule(const std::vector<char> & code);

    public:
        void CreateDescriptorSetLayout();
        void CreateGraphicsPipeline();

    public:
        void CreatePipeline(VkDevice &            device,
                            VkExtent2D            extent,
                            VkRenderPass &        renderPass,
                            VkSampleCountFlagBits msaaSamples,
                            const char *          vertexFile,
                            const char *          fragmentFile,
                            PipelineType          type);

        void CreatePipeline(VkDevice &            device,
                            VkExtent2D            extent,
                            VkRenderPass &        renderPass,
                            VkSampleCountFlagBits msaaSamples);
        void DestroyPipeline();

    public:
        VkDescriptorSetLayout & GetDescriptorSetLayout();
        VkPipeline &            GetPipeline();
        VkPipelineLayout &      GetPieplineLayout();
        void                    BindImage(Image & texture, DescriptorPool & pool, VkDescriptorSet * descriptorSet);

    public:
        bool operator==(GraphicsPipeline & otherObject);
    };
} // namespace sckz