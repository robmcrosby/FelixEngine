#include <VulkanTypes.hpp>

#ifndef VulkanLayoutSet_hpp
#define VulkanLayoutSet_hpp


class VulkanLayoutSet {
private:
  VulkanDevice*    mDevice;
  int              mFrames;
  VulkanLayouts    mLayouts;
  VkPipelineLayout mVkPipelineLayout;

public:
  VulkanLayoutSet(VulkanDevice* device, int frames);
  ~VulkanLayoutSet();

  VulkanLayoutPtr at(int index);

  void getVkDescriptorSetLayouts(VkDescriptorSetLayouts& setLayouts);
  void getVkDescriptorSets(VkDescriptorSets& descriptorSets, int frame);
  VkPipelineLayout getVkPipelineLayout();

  void destroy();

private:
  VkPipelineLayout createVkPipelineLayout(VkDescriptorSetLayouts& setLayouts);
  void destroyPipelineLayout();
};


#endif /* VulkanLayoutSet_hpp */
