#include "VulkanIncludes.hpp"
#include <fstream>

using namespace std;


VulkanCommand::VulkanCommand(VulkanQueue* queue):
  mQueue(queue),
  mKernal(VK_NULL_HANDLE),
  mVkPipeline(VK_NULL_HANDLE),
  mVkDescriptorSetLayout(VK_NULL_HANDLE),
  mVkPipelineLayout(VK_NULL_HANDLE) {

}

VulkanCommand::~VulkanCommand() {
  destroy();
}

void VulkanCommand::clearShaders() {
  if (mKernal != VK_NULL_HANDLE) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkDestroyShaderModule(device, mKernal, nullptr);
    mKernal = VK_NULL_HANDLE;
  }
}

void VulkanCommand::clearBindings() {
  mLayoutBindings.clear();
  destroyDescriptorSetLayout();
}

void VulkanCommand::destroyDescriptorSetLayout() {
  if (mVkDescriptorSetLayout != VK_NULL_HANDLE) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkDestroyDescriptorSetLayout(device, mVkDescriptorSetLayout, nullptr);
    mVkDescriptorSetLayout = VK_NULL_HANDLE;
  }
}

void VulkanCommand::destroyPipelineLayout() {
  if (mVkPipelineLayout != VK_NULL_HANDLE) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkDestroyPipelineLayout(device, mVkPipelineLayout, nullptr);
    mVkPipelineLayout = VK_NULL_HANDLE;
  }
}

void VulkanCommand::destroyPipeline() {
  if (mVkPipeline != VK_NULL_HANDLE) {
    VkDevice device = mQueue->getDevice()->getVkDevice();
    vkDestroyPipeline(device, mVkPipeline, nullptr);
    mVkPipeline = VK_NULL_HANDLE;
  }
}

void VulkanCommand::destroy() {
  clearShaders();
  destroyPipelineLayout();
  clearBindings();
  destroyPipeline();
}

bool VulkanCommand::setKernal(StringRef filename, StringRef entry) {
  SPIRVCode code;
  return readSPIRV(code, filename) && setKernal(code, entry);
}

bool VulkanCommand::setKernal(SPIRVCodeRef code, StringRef entry) {
  VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  mKernalEntry = entry;
  VkDevice device = mQueue->getDevice()->getVkDevice();
  return vkCreateShaderModule(device, &createInfo, nullptr, &mKernal) == VK_SUCCESS;
}

void VulkanCommand::setBuffer(VulkanBufferPtr buffer, uint32_t binding) {
  LayoutBinding layoutBinding;
  layoutBinding.buffer = buffer;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  mLayoutBindings.push_back(layoutBinding);
}

bool VulkanCommand::initPipeline() {
  if (!initPipelineLayout())
    return false;

  VkComputePipelineCreateInfo computePipelineCreateInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
  computePipelineCreateInfo.pNext = nullptr;
  computePipelineCreateInfo.flags = 0;
  computePipelineCreateInfo.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  computePipelineCreateInfo.stage.pNext = nullptr;
  computePipelineCreateInfo.stage.flags = 0;
  computePipelineCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  computePipelineCreateInfo.stage.module = mKernal;
  computePipelineCreateInfo.stage.pName = mKernalEntry.c_str();
  computePipelineCreateInfo.stage.pSpecializationInfo = nullptr;
  computePipelineCreateInfo.layout = mVkPipelineLayout;
  computePipelineCreateInfo.basePipelineHandle = 0;
  computePipelineCreateInfo.basePipelineIndex = 0;

  VkDevice device = mQueue->getDevice()->getVkDevice();
  if (vkCreateComputePipelines(device, 0, 1, &computePipelineCreateInfo, 0, &mVkPipeline) != VK_SUCCESS) {
    cerr << "Error creating Pipeline";
    return false;
  }
  return true;
}

bool VulkanCommand::initDescriptorSetLayout() {
  vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
  for (auto layoutBinding : mLayoutBindings)
    descriptorSetLayoutBindings.push_back(layoutBinding.binding);

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
  descriptorSetLayoutCreateInfo.pNext = nullptr;
  descriptorSetLayoutCreateInfo.flags = 0;
  descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
  descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();

  VkDevice device = mQueue->getDevice()->getVkDevice();
  if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, 0, &mVkDescriptorSetLayout) != VK_SUCCESS) {
    cerr << "Error creating Descriptor Set Layout" << endl;
    return false;
  }
  return true;
}

bool VulkanCommand::initPipelineLayout() {
  if (!initDescriptorSetLayout())
    return false;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipelineLayoutCreateInfo.pNext = nullptr;
  pipelineLayoutCreateInfo.flags = 0;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &mVkDescriptorSetLayout;
  pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
  pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

  VkDevice device = mQueue->getDevice()->getVkDevice();
  if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, 0, &mVkPipelineLayout) != VK_SUCCESS) {
    cerr << "Error creating Pipeline Layout" << endl;
    return false;
  }
  return true;
}

bool VulkanCommand::readSPIRV(SPIRVCode& code, StringRef filename) {
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
