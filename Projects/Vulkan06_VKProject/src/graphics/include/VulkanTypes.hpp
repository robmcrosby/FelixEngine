#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <vk_mem_alloc.h>

#ifndef VulkanTypes_hpp
#define VulkanTypes_hpp

class SDL_Window;

typedef const char* CString;
typedef std::vector<CString> CStrings;
typedef std::vector<std::string> Strings;
typedef const std::string& StringRef;

typedef std::vector<uint32_t> SPIRVCode;
typedef const SPIRVCode& SPIRVCodeRef;

typedef std::vector<VkLayerProperties>            VulkanLayerProperties;
typedef std::vector<VkExtensionProperties>        VulkanExtensionProperties;
typedef std::vector<VkQueueFamilyProperties>      VulkanQueueFamilies;
typedef std::vector<VkDescriptorSet>              VkDescriptorSets;
typedef std::vector<VkDescriptorSetLayout>        VkDescriptorSetLayouts;
typedef std::vector<VkDescriptorSetLayoutBinding> VkDescriptorSetLayoutBindings;
typedef std::vector<VkAttachmentReference>        VkAttachmentReferences;
typedef std::vector<VkAttachmentDescription>      VkAttachmentDescriptions;
typedef std::vector<VkDescriptorSetLayout>        VkDescriptorSetLayouts;
typedef std::vector<VkDescriptorSet>              VkDescriptorSets;
typedef std::vector<VkImageMemoryBarrier>         VkImageMemoryBarriers;
typedef std::vector<VkBuffer>                     VkBuffers;
typedef std::vector<VkImage>                      VkImages;
typedef std::vector<VkImageView>                  VkImageViews;
typedef std::vector<VkFramebuffer>                VkFramebuffers;
typedef std::vector<VkRenderPass>                 VkRenderPasses;
typedef std::vector<VkDeviceSize>                 VkDeviceSizes;
typedef std::vector<VkSurfaceFormatKHR>           VkSurfaceFormats;
typedef std::vector<VkPresentModeKHR>             VkPresentModes;
typedef std::vector<VkCommandBuffer>              VkCommandBuffers;
typedef std::vector<VkSemaphore>                  VkSemaphores;
typedef std::vector<VkFence>                      VkFences;
typedef std::vector<VkDescriptorBufferInfo>       VkDescriptorBufferInfos;
typedef std::vector<VkDescriptorImageInfo>        VkDescriptorImageInfos;
typedef std::vector<VmaAllocation>                VmaAllocations;

typedef std::vector<VkVertexInputBindingDescription>   VkVertexInputBindingDescriptions;
typedef std::vector<VkVertexInputAttributeDescription> VkVertexInputAttributeDescriptions;

typedef std::map<uint32_t, VkSampler> VkSamplerMap;

typedef std::function<void (int)> VulkanRecording;

struct LayoutBinding {
  std::string name;
  VkDescriptorSetLayoutBinding binding;
};
typedef std::vector<LayoutBinding>    LayoutBindingSet;
typedef std::vector<LayoutBindingSet> LayoutBindingSets;


class VulkanDevice;
typedef std::shared_ptr<VulkanDevice> VulkanDevicePtr;
typedef std::vector<VulkanDevicePtr> VulkanDevices;

class VulkanBuffer;
typedef std::shared_ptr<VulkanBuffer> VulkanBufferPtr;
typedef std::map<uint32_t, VulkanBufferPtr> VulkanBufferMap;

class VulkanImage;
typedef std::shared_ptr<VulkanImage> VulkanImagePtr;
typedef std::vector<VulkanImagePtr> VulkanImages;

class VulkanSampler;
typedef std::shared_ptr<VulkanSampler> VulkanSamplerPtr;
typedef std::vector<VulkanSamplerPtr> VulkanSamplers;

class VulkanLayout;
typedef std::shared_ptr<VulkanLayout> VulkanLayoutPtr;
typedef std::vector<VulkanLayoutPtr> VulkanLayouts;

class VulkanLayoutSet;
typedef std::shared_ptr<VulkanLayoutSet> VulkanLayoutSetPtr;

class VulkanMesh;
typedef std::shared_ptr<VulkanMesh> VulkanMeshPtr;

class VulkanFrameBuffer;
typedef std::shared_ptr<VulkanFrameBuffer> VulkanFrameBufferPtr;

class VulkanRenderPass;
typedef std::shared_ptr<VulkanRenderPass> VulkanRenderPassPtr;

class VulkanShader;
typedef std::shared_ptr<VulkanShader> VulkanShaderPtr;

class VulkanPipeline;
typedef std::shared_ptr<VulkanPipeline> VulkanPipelinePtr;

class VulkanQueue;
typedef std::shared_ptr<VulkanQueue> VulkanQueuePtr;
typedef std::vector<VulkanQueuePtr> VulkanQueues;

class VulkanCommand;
typedef std::shared_ptr<VulkanCommand> VulkanCommandPtr;
typedef std::vector<VulkanCommandPtr> VulkanCommands;

class VulkanSwapChain;
typedef std::shared_ptr<VulkanSwapChain> VulkanSwapChainPtr;

class VulkanFrameSync;
typedef std::shared_ptr<VulkanFrameSync> VulkanFrameSyncPtr;

struct BufferLayoutBinding {
  VulkanBufferPtr buffer;
  VkDescriptorSetLayoutBinding binding;
};
typedef std::vector<BufferLayoutBinding> BufferLayoutBindings;

struct TextureLayoutBinding {
  VulkanImagePtr image;
  VulkanSamplerPtr sampler;
  VkDescriptorSetLayoutBinding binding;
};
typedef std::vector<TextureLayoutBinding> TextureLayoutBindings;


#endif /* VulkanTypes_hpp */
