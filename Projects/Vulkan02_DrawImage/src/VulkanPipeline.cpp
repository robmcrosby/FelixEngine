#include "VulkanIncludes.hpp"


using namespace std;


VulkanPipeline::VulkanPipeline(VulkanDevice* device):
  mDevice(device),
  mVkPipeline(VK_NULL_HANDLE),
  mVkPipelineLayout(VK_NULL_HANDLE) {

}

VulkanPipeline::~VulkanPipeline() {
  destroy();
}

bool VulkanPipeline::setKernal(StringRef filename, StringRef entry) {
  mKernal = mDevice->createShader(VK_SHADER_STAGE_COMPUTE_BIT);
  return mKernal->load(filename, entry);
}

void VulkanPipeline::clearShaders() {
  mKernal = nullptr;
}

VkPipeline VulkanPipeline::getVkPipeline() {
  return getVkPipeline(getVkDescriptorSetLayouts());
}

VkPipeline VulkanPipeline::getVkPipeline(VkDescriptorSetLayouts& setLayouts) {
  if (mVkPipeline == VK_NULL_HANDLE)
    mVkPipeline = createVkPipeline(setLayouts);
  return mVkPipeline;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout() {
  return getVkPipelineLayout(getVkDescriptorSetLayouts());
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout(VkDescriptorSetLayouts& setLayouts) {
  if (mVkPipelineLayout == VK_NULL_HANDLE)
    mVkPipelineLayout = createVkPipelineLayout(setLayouts);
  return mVkPipelineLayout;
}

void VulkanPipeline::destroy() {
  clearShaders();
  destroyPipelineLayout();
  destroyPipeline();
  clearVkDescriptorSetLayouts();
}

void VulkanPipeline::destroyPipelineLayout() {
  if (mVkPipelineLayout != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyPipelineLayout(device, mVkPipelineLayout, nullptr);
    mVkPipelineLayout = VK_NULL_HANDLE;
  }
}

void VulkanPipeline::destroyPipeline() {
  if (mVkPipeline != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyPipeline(device, mVkPipeline, nullptr);
    mVkPipeline = VK_NULL_HANDLE;
  }
}

VkDescriptorSetLayouts& VulkanPipeline::getVkDescriptorSetLayouts() {
  if (mVkDescriptorSetLayouts.size() == 0) {
    VkDevice device = mDevice->getVkDevice();
    LayoutBindingSets layoutBindingSets;
    getLayoutBindingSets(layoutBindingSets);
    for (const auto& layoutSet : layoutBindingSets) {
      VkDescriptorSetLayoutBindings bindings;
      for (const auto& layout : layoutSet)
        bindings.push_back(layout.binding);

      VkDescriptorSetLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, 0, 0};
      createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
      createInfo.pBindings = bindings.data();

      VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
      if (vkCreateDescriptorSetLayout(device, &createInfo, 0, &descriptorSetLayout) != VK_SUCCESS)
        cerr << "Error creating Descriptor Set Layout" << endl;
      mVkDescriptorSetLayouts.push_back(descriptorSetLayout);
    }
  }
  return mVkDescriptorSetLayouts;
}

void VulkanPipeline::clearVkDescriptorSetLayouts() {
  VkDevice device = mDevice->getVkDevice();
  for (auto setLayout : mVkDescriptorSetLayouts)
    vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
  mVkDescriptorSetLayouts.clear();
}

void VulkanPipeline::getLayoutBindingSets(LayoutBindingSets& layoutBindingSets) {
  layoutBindingSets = mKernal->getLayoutBindingSets();
}

VkPipeline VulkanPipeline::createVkPipeline(VkDescriptorSetLayouts& setLayouts) {
  VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
  createInfo.stage = mKernal->getVkPipelineShaderStageCreateInfo();
  createInfo.layout = getVkPipelineLayout(setLayouts);
  createInfo.basePipelineHandle = 0;
  createInfo.basePipelineIndex = 0;

  VkDevice device = mDevice->getVkDevice();
  VkPipeline pipeline = VK_NULL_HANDLE;
  if (vkCreateComputePipelines(device, 0, 1, &createInfo, 0, &pipeline) != VK_SUCCESS)
    cerr << "Error creating Pipeline";
  return pipeline;
}

VkPipelineLayout VulkanPipeline::createVkPipelineLayout(VkDescriptorSetLayouts& setLayouts) {
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
