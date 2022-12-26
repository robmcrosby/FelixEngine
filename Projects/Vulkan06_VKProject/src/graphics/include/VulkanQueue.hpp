#include <VulkanTypes.hpp>

#ifndef VulkanQueue_hpp
#define VulkanQueue_hpp


class VulkanQueue {
private:
  VulkanDevice* mDevice;
  VkQueueFlags  mVkQueueFlags;
  uint32_t      mFamilyIndex;
  uint32_t      mQueueIndex;
  VkQueue       mVkQueue;
  VkCommandPool mVkCommandPool;

public:
  VulkanQueue(VulkanDevice* device, VkQueueFlags flags, uint32_t familyIndex, uint32_t queueIndex);
  ~VulkanQueue();

  uint32_t familyIndex() const {return mFamilyIndex;}
  uint32_t queueIndex() const {return mQueueIndex;}

  VulkanDevice* getDevice() const {return mDevice;}

  VkQueue getVkQueue() const {return mVkQueue;}
  VkCommandPool getVkCommandPool() const {return mVkCommandPool;}

  bool init();
  void destroy();

  bool initalized() const {return mVkQueue != VK_NULL_HANDLE;}

  void transition(
    VulkanImagePtr image,
    VkImageLayout newImageLayout,
    VkAccessFlags dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyBufferToImage(VulkanBufferPtr buffer, VulkanImagePtr image);
  void copyImageToBuffer(VulkanImagePtr image, VulkanBufferPtr buffer);
  void copyBufferToBuffer(VulkanBufferPtr src, VulkanBufferPtr dst);

  VulkanCommandPtr createCommand(int frames = 1);
  VulkanCommandPtr beginSingleCommand();

  void submitCommand(VulkanCommandPtr command);
  void submitCommand(VulkanCommandPtr command, VulkanFrameSyncPtr frameSync);
  void waitIdle();

private:
  void destroyCommandPool();
};


#endif /* VulkanQueue_hpp */
