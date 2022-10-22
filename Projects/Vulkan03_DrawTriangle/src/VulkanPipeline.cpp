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

bool VulkanPipeline::setKernalShader(StringRef filename, StringRef entry) {
  mKernalShader = mDevice->createShader();
  return mKernalShader->load(filename, entry);
}

bool VulkanPipeline::setVertexShader(StringRef filename, StringRef entry) {
  mVertexShader = mDevice->createShader();
  return mVertexShader->load(filename, entry);
}

bool VulkanPipeline::setFragmentShader(StringRef filename, StringRef entry) {
  mFragmentShader = mDevice->createShader();
  return mFragmentShader->load(filename, entry);
}

void VulkanPipeline::clearShaders() {
  mKernalShader = nullptr;
  mVertexShader = nullptr;
  mFragmentShader = nullptr;
}

VkPipeline VulkanPipeline::getVkPipeline(VulkanSetLayoutPtr layout) {
  if (mVkPipeline == VK_NULL_HANDLE) {
    createComputePipeline(layout);
  }
  return mVkPipeline;
}

VkPipelineLayout VulkanPipeline::getVkPipelineLayout(VulkanSetLayoutPtr layout) {
  if (mVkPipelineLayout == VK_NULL_HANDLE) {
    VkDescriptorSetLayout setLayout = layout->getVkDescriptorSetLayout();
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

void VulkanPipeline::createComputePipeline(VulkanSetLayoutPtr layout) {
  auto pipelineLayout = getVkPipelineLayout(layout);
  if (pipelineLayout != VK_NULL_HANDLE) {
    VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
    createInfo.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    createInfo.stage.pNext = nullptr;
    createInfo.stage.flags = 0;
    createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    createInfo.stage.module = mKernalShader->getVkShaderModule();
    createInfo.stage.pName = mKernalShader->getEntryFunction();
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
