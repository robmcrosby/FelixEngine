#include <VulkanTypes.hpp>

#ifndef VulkanImage_hpp
#define VulkanImage_hpp


class VulkanImage {
private:
  VulkanDevice*  mDevice;
  VkFormat       mVkFormat;
  VkImages       mVkImages;
  VmaAllocations mVmaAllocations;
  VkImageViews   mVkImageViews;
  uint32_t       mWidth;
  uint32_t       mHeight;

  VkImageUsageFlags        mVkImageUsageFlags;
  VmaMemoryUsage           mVmaMemoryUsage;
  VmaAllocationCreateFlags mVmaCreateFlags;
  VkDescriptorImageInfos   mVkDescriptorImageInfos;

  VkImageLayout        mCurImageLayout;
  VkPipelineStageFlags mCurStageMask;
  VkAccessFlags        mCurAccessMask;

  VkImageMemoryBarriers mActiveBerriers;

public:
  VulkanImage(VulkanDevice* device);
  ~VulkanImage();

  void setUsage(VkImageUsageFlags flags);
  void setCreateFlags(VmaAllocationCreateFlags flags);

  bool alloc(uint32_t width, uint32_t height, int frames = 1);
  void destroy();
  void clearImages();

  void setSwapImages(const VkImages& images, VkFormat format, uint32_t width, uint32_t height);

  bool load(VulkanQueuePtr queue, StringRef filepath);
  bool load(VulkanQueuePtr queue, const void* data, int width, int height, VkFormat format);

  VkImage     getVkImage(int index = 0) const {return mVkImages.at(index);}
  VkImageView getVkImageView(int index = 0) const {return mVkImageViews.at(index);}
  VkFormat    getVkFormat() const {return mVkFormat;}

  VmaAllocationInfo getVmaAllocationInfo(int index = 0) const;
  void* data(int index = 0);

  VkDeviceSize formatSize() const;
  static VkDeviceSize formatSize(VkFormat format);

  VkDeviceSize size() const {return mWidth * mHeight * formatSize();}
  uint32_t width() const {return mWidth;}
  uint32_t height() const {return mHeight;}
  uint32_t frames() const {return static_cast<uint32_t>(mVkImages.size());}
  bool isSwapImage() const {return mVmaAllocations.empty() && !mVkImages.empty();}

  VkDescriptorImageInfo* getVkDescriptorImageInfo(int index = 0) {
    return &mVkDescriptorImageInfos.at(index % frames());
  }

  void transition(
    VkCommandBuffer      commandBuffer,
    VkImageLayout        newImageLayout,
    VkAccessFlags        dstAccessMask,
    VkPipelineStageFlags dstStageMask
  );
  void copyFromBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame = 0);
  void copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame = 0);

private:
  bool allocVkImage(
    VkImage&       image,
    VmaAllocation& allocation,
    uint32_t       width,
    uint32_t       height
  ) const;
  VkImageView createImageView(
    VkImage            image,
    VkFormat           format,
    VkImageAspectFlags aspectFlags,
    int32_t            mipLevels
  ) const;
};


#endif /* VulkanImage_hpp */
