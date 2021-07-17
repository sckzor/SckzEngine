#pragma once
#include "../../Include.hpp"
#include "DescriptorPool.hpp"
#include "Fbo.hpp"
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
            MODEL_PIPELINE,
            PARTICLE_PIPELINE,
            COMBINE_PIPELINE,
            CUBEMAP_PIPELINE
        };

    private:
        VkDescriptorSetLayout complexDescriptorSetLayout;
        VkPipelineLayout      complexPipelineLayout;
        VkPipeline            complexPipeline;

        VkDescriptorSetLayout simpleDescriptorSetLayout;
        VkPipelineLayout      simplePipelineLayout;
        VkPipeline            simplePipeline;

        VkDevice * device;

        Fbo *        complexFbo;
        const char * complexVertexFile;
        const char * complexFragmentFile;

        Fbo *        simpleFbo;
        const char * simpleVertexFile;
        const char * simpleFragmentFile;

        PipelineType type;

        uint32_t complexVertexUboCount   = -1;
        uint32_t complexSamplerCount     = -1;
        uint32_t complexFragmentUboCount = -1;

        bool     hasSimple            = false;
        uint32_t simpleVertexUboCount = -1;
        uint32_t simpleSamplerCount   = -1;

    private:
        static std::vector<char> ReadFile(const std::string & filename);
        VkShaderModule           CreateShaderModule(const std::vector<char> & code);

    private:
        void CreateComplexDescriptorSetLayout();
        void CreateSimpleDescriptorSetLayout();
        void CreateGraphicsPipeline(VkPipelineLayout &      pipelineLayout,
                                    VkPipeline &            pipeline,
                                    VkDescriptorSetLayout & descriptorSetLayout,
                                    Fbo &                   fbo,
                                    const char *            vertexFile,
                                    const char *            fragmentFile);

    public:
        void CreatePipeline(VkDevice &   device,
                            Fbo &        complexFbo,
                            const char * vertexFile,
                            const char * fragmentFile,
                            Fbo &        simpleFbo,
                            const char * simpleVertexFile,
                            const char * simpleFragmentFile,
                            PipelineType type);

        void CreatePipeline(VkDevice & device, Fbo & complexFbo, Fbo & simpleFbo);
        void DestroyPipeline();

    public:
        VkDescriptorSetLayout & GetComplexDescriptorSetLayout();
        VkPipeline &            GetComplexPipeline();
        VkPipelineLayout &      GetComplexPipelineLayout();
        Fbo &                   GetComplexFbo();

        VkDescriptorSetLayout & GetSimpleDescriptorSetLayout();
        VkPipeline &            GetSimplePipeline();
        VkPipelineLayout &      GetSimplePieplineLayout();
        Fbo &                   GetSimpleFbo();

        void BindComplexShaderData(Buffer::SubBlock  vUboInfo[],
                                   Image             samplerInfo[],
                                   Buffer::SubBlock  fUboInfo[],
                                   DescriptorPool &  pool,
                                   VkDescriptorSet * descriptorSet);

        void BindSimpleShaderData(Buffer::SubBlock  vUboInfo[],
                                  Image             samplerInfo[],
                                  DescriptorPool &  pool,
                                  VkDescriptorSet * descriptorSet);

    public:
        bool operator==(GraphicsPipeline & otherObject);
    };
} // namespace sckz