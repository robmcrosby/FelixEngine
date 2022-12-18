#include <VulkanTypes.hpp>

#ifndef VulkanSampler_hpp
#define VulkanSampler_hpp


class VulkanSampler {
private:
  VulkanDevice* mDevice;
  VkSampler mVkSampler;
  VkFilter  mMinFilter;
  VkFilter  mMagFilter;
  VkFilter  mMipFilter;
  uint32_t  mMipLevels;

public:
  VulkanSampler(VulkanDevice* device);
  ~VulkanSampler();

  void setFilters(VkFilter min, VkFilter mag, VkFilter mip = VK_FILTER_NEAREST);

  VkSampler getVkSampler();

private:
  VkSampler createVkSampler() const;
  uint32_t getKey() const;
};


#endif /* VulkanSampler_hpp */
