#include "VulkanIncludes.hpp"


using namespace std;


VulkanLayoutSet::VulkanLayoutSet(VulkanDevice* device):
  mDevice(device),
  mVkPipelineLayout(VK_NULL_HANDLE) {

}

VulkanLayoutSet::~VulkanLayoutSet() {
  destroy();
}

VulkanLayoutPtr VulkanLayoutSet::at(int index) {
  while (mLayouts.size() <= index)
    mLayouts.push_back(mDevice->createLayout());
  return mLayouts.at(index);
}

void VulkanLayoutSet::getVkDescriptorSetLayouts(VkDescriptorSetLayouts& setLayouts) {
  for (auto layout : mLayouts)
    setLayouts.push_back(layout->getVkDescriptorSetLayout());
}

void VulkanLayoutSet::getVkDescriptorSets(VkDescriptorSets& descriptorSets) {
  for (auto layout : mLayouts)
    descriptorSets.push_back(layout->getVkDescriptorSet());
}

VkPipelineLayout VulkanLayoutSet::getVkPipelineLayout() {
  if (mVkPipelineLayout == VK_NULL_HANDLE) {
    VkDescriptorSetLayouts setLayouts;
    getVkDescriptorSetLayouts(setLayouts);
    mVkPipelineLayout = createVkPipelineLayout(setLayouts);
  }
  return mVkPipelineLayout;
}

void VulkanLayoutSet::destroy() {
  destroyPipelineLayout();
  mLayouts.clear();
}

VkPipelineLayout VulkanLayoutSet::createVkPipelineLayout(VkDescriptorSetLayouts& setLayouts) {
  VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0};
  createInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
  createInfo.pSetLayouts = setLayouts.data();
  createInfo.pushConstantRangeCount = 0;
  createInfo.pPushConstantRanges = nullptr;

  VkDevice device = mDevice->getVkDevice();
  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  if (vkCreatePipelineLayout(device, &createInfo, 0, &pipelineLayout) != VK_SUCCESS)
    cerr << "Error creating Pipeline Layout" << endl;
  return pipelineLayout;
}

void VulkanLayoutSet::destroyPipelineLayout() {
  if (mVkPipelineLayout != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyPipelineLayout(device, mVkPipelineLayout, nullptr);
    mVkPipelineLayout = VK_NULL_HANDLE;
  }
}
