#include "VulkanIncludes.hpp"
#include <fstream>


using namespace std;


VulkanPipeline::VulkanPipeline(VulkanDevice* device):
  mDevice(device),
  mVkPipeline(VK_NULL_HANDLE),
  mVkPipelineLayout(VK_NULL_HANDLE),
  mKernal(VK_NULL_HANDLE) {

}

VulkanPipeline::~VulkanPipeline() {
  destroy();
}

bool VulkanPipeline::setKernal(StringRef filename, StringRef entry) {
  SPIRVCode code;
  return readSPIRV(code, filename) && setKernal(code, entry);
}

bool VulkanPipeline::setKernal(SPIRVCodeRef code, StringRef entry) {
  VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  mKernalEntry = entry;
  VkDevice device = mDevice->getVkDevice();
  return vkCreateShaderModule(device, &createInfo, nullptr, &mKernal) == VK_SUCCESS;
}

void VulkanPipeline::clearShaders() {
  if (mKernal != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyShaderModule(device, mKernal, nullptr);
    mKernal = VK_NULL_HANDLE;
  }
}

VkPipeline VulkanPipeline::getVkPipeline(VulkanSetLayoutPtr layout) {
  if (mVkPipeline == VK_NULL_HANDLE) {
    auto pipelineLayout = getVkPipelineLayout(layout);
    if (pipelineLayout != VK_NULL_HANDLE) {
      VkComputePipelineCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
      createInfo.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
      createInfo.stage.pNext = nullptr;
      createInfo.stage.flags = 0;
      createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
      createInfo.stage.module = mKernal;
      createInfo.stage.pName = mKernalEntry.c_str();
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

bool VulkanPipeline::readSPIRV(SPIRVCode& code, StringRef filename) {
  ifstream file(filename, std::ios::ate | std::ios::binary);
  if (file.is_open()) {
    size_t fileSize = (size_t)file.tellg();
    code.resize(fileSize/sizeof(uint32_t));

    file.seekg(0);
    file.read((char*)code.data(), code.size() * sizeof(uint32_t));

    file.close();
    return true;
  }
  cerr << "Error reading SPIR-V File: " << filename << endl;
  return false;
}
