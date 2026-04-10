#include <FelixEngine/VulkanTypes.hpp>

#ifndef VulkanLayoutSet_hpp
#define VulkanLayoutSet_hpp


namespace Felix {

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
  int add(VulkanLayoutPtr layout);

  void getVkDescriptorSetLayouts(VkDescriptorSetLayouts& setLayouts);
  void getVkDescriptorSets(VkDescriptorSets& descriptorSets, int frame);
  VkPipelineLayout getVkPipelineLayout();

  void destroy();

private:
  VkPipelineLayout createVkPipelineLayout(VkDescriptorSetLayouts& setLayouts);
  void destroyPipelineLayout();
};

} /* Felix */

#endif /* VulkanLayoutSet_hpp */
