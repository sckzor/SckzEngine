#pragma once
#include "../Include.hpp"
#include "MemoryHelper.hpp"
#include "Buffer.hpp"
#include "CommandBuffer.hpp"

namespace sckz {
    class Image {
    private:
        VkImage            image;
        VkDeviceMemory     imageMemory;
        VkImageView        imageView;
        VkSampler          sampler;
        VkDevice         * device;
        VkPhysicalDevice * physicalDevice; 
        VkFormat           format;
        VkQueue          * queue;

        uint32_t        mipLevels;

    public:
        void CreateImage                              (uint32_t               width, 
                                                       uint32_t               height, 
                                                       uint32_t               mipLevels, 
                                                       VkSampleCountFlagBits  numSamples, 
                                                       VkFormat               format, 
                                                       VkImageTiling          tiling, 
                                                       VkImageUsageFlags      usage, 
                                                       VkMemoryPropertyFlags  properties, 
                                                       VkDevice         &     device, 
                                                       VkPhysicalDevice &     physicalDevice,
                                                       VkQueue          &     queue);

        void CreateImage                              (VkDevice         &     device,
                                                       VkImage          &     image,
                                                       VkFormat         &     imageFormat,
                                                       uint32_t               mipLevels);
        
        void CreateImageView                          (VkImageAspectFlags     aspectFlags);

        void CreateTextureImage                       (const char       *     fileName, 
                                                       VkDevice         &     device,
                                                       VkPhysicalDevice &     physicalDevice,
                                                       VkCommandPool    &     pool,
                                                       VkQueue          &     queue);

        void GenerateMipmaps                          (VkFormat               imageFormat, 
                                                       int32_t                texWidth, 
                                                       int32_t                texHeight,
                                                       VkCommandPool    &     pool);
        
        void TransitionImageLayout                    (VkImageLayout          oldLayout, 
                                                       VkImageLayout          newLayout,
                                                       VkCommandPool    &     pool);

        VkSampleCountFlagBits GetMaxUsableSampleCount ();
        
        void CreateTextureSampler                     ();

    public: 
        VkImageView & GetImageView   ();
        VkSampler   & GetSampler     ();
        VkFormat      GetFormat      ();
    };
}