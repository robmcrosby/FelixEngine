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

bool VulkanPipeline::init() {
  return false;
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
