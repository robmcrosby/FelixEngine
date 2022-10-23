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
  mKernal = mDevice->createShader();
  return mKernal->load(filename, entry);
}

void VulkanPipeline::clearShaders() {
  mKernal = nullptr;
}

VkPipeline VulkanPipeline::getVkPipeline(VkDescriptorSetLayout setLayout) {
  if (mVkPipeline == VK_NULL_HANDLE) {
    auto pipelineLayout = getVkPipelineLayout(setLayout);
    if (pipelineLayout != VK_NULL_HANDLE) {
      VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
      createInfo.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
      createInfo.stage.pNext = nullptr;
      createInfo.stage.flags = 0;
      createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
      createInfo.stage.module = mKernal->getVkShaderModule();
      createInfo.stage.pName = mKernal->getEntryFunction();
      createInfo.stage.pSpecializationInfo = nullptr;
      createInfo.layout = pipelineLayout;
      createInfo.basePipelineHandle = 0;
      createInfo.basePipelineIndex = 0;

      VkDevice device = mDevice->getVkDevice();
      if (vkCreateComputePipelines(device, 0, 1, &createInfo, 0, &mVkPipeline) != VK_SUCCESS) {
        cerr << "Error creating Pipeline";
        mVkPipeline = VK_NULL_HANDLE;
      }
    }
  }
  return mVkPipeline;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout(VkDescriptorSetLayout setLayout) {
  if (mVkPipelineLayout == VK_NULL_HANDLE) {
    //VkDescriptorSetLayout setLayout = layout->getVkDescriptorSetLayout();
    if (setLayout != VK_NULL_HANDLE) {
      VkPipelineLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, 0, 0};
      createInfo.setLayoutCount = 1;
      createInfo.pSetLayouts = &setLayout;
      createInfo.pushConstantRangeCount = 0;
      createInfo.pPushConstantRanges = nullptr;

      VkDevice device = mDevice->getVkDevice();
      if (vkCreatePipelineLayout(device, &createInfo, 0, &mVkPipelineLayout) != VK_SUCCESS) {
        cerr << "Error creating Pipeline Layout" << endl;
        mVkPipelineLayout = VK_NULL_HANDLE;
      }
    }
  }
  return mVkPipelineLayout;
}

void VulkanPipeline::destroy() {
  clearShaders();
  destroyPipelineLayout();
  destroyPipeline();
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
