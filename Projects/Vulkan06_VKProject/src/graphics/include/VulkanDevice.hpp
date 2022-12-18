#include <VulkanTypes.hpp>
#include <VulkanInstance.hpp>

#ifndef VulkanDevice_hpp
#define VulkanDevice_hpp


class VulkanDevice {
private:
  VkPhysicalDevice mVkPhysicalDevice;
  VkDevice         mVkDevice;

  VmaAllocator mVmaAllocator;

  VkPhysicalDeviceProperties mProperties;
  VkPhysicalDeviceFeatures   mFeatures;
  VulkanExtensionProperties  mAvailableExtensions;
  CStrings                   mEnabledExtensions;
  VulkanQueueFamilies        mQueueFamilies;
  std::vector<uint32_t>      mQueueFamilyCounts;

  VkPhysicalDeviceMemoryProperties mMemoryProperties;

  VulkanQueues mQueues;
  VkSamplerMap mVkSamplerMap;

public:
  VulkanDevice(VkPhysicalDevice device);
  ~VulkanDevice();

  bool init();
  void destroy();

  std::string name() const;
  std::string type() const;

  VkInstance getVkInstance() const {return VulkanInstance::Get().getVkInstance();}
  VkPhysicalDevice getVkPhysicalDevice() const {return mVkPhysicalDevice;}
  VkPhysicalDeviceType deviceType() const {return mProperties.deviceType;}
  VkDevice getVkDevice() const {return mVkDevice;}
  VmaAllocator getVmaAllocator() const {return mVmaAllocator;}

  bool addExtension(StringRef extension);
  const CStrings& enabledExtensions() const {return mEnabledExtensions;}

  VulkanQueuePtr createQueue(VkQueueFlags flags);

  VulkanBufferPtr      createBuffer();
  VulkanImagePtr       createImage();
  VulkanSamplerPtr     createSampler();
  VulkanLayoutPtr      createLayout(int frames = 1);
  VulkanLayoutSetPtr   createLayoutSet(int frames = 1);
  VulkanMeshPtr        createMesh();
  VulkanFrameBufferPtr createFrameBuffer();
  VulkanRenderPassPtr  createRenderPass();
  VulkanShaderPtr      createShader(VkShaderStageFlagBits stage);
  VulkanPipelinePtr    createPipeline();
  VulkanSwapChainPtr   createSwapChain();
  VulkanFrameSyncPtr   createFrameSync();

  bool getVkMemoryType(uint32_t& index, VkMemoryType& type, VkMemoryPropertyFlags properties);

  friend std::ostream& operator<<(std::ostream& os, const VulkanDevicePtr& device);
  std::ostream& print(std::ostream& os) const;
  std::ostream& printQueueFlags(std::ostream& os, VkQueueFlags flags) const;
  std::ostream& printHeapFlags(std::ostream& os, VkMemoryHeapFlags flags) const;
  std::ostream& printMemoryFlags(std::ostream& os, VkMemoryPropertyFlags flags) const;

  VkSamplerMap& getVkSamplerMap() {return mVkSamplerMap;}
  void clearVkSamplerMap();

private:
  bool initVmaAllocator();
  void destoryVmaAllocator();

  bool pickQueueFamily(VkQueueFlags flags, uint32_t& familyIndex, uint32_t& queueIndex);
  void clearQueues();
};


#endif /* VulkanDevice_hpp */
