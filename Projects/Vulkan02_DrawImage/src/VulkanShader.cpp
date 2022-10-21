#include "VulkanIncludes.hpp"
#include <fstream>


using namespace std;

VulkanShader::VulkanShader(VulkanDevice* device):
  mDevice(device),
  mVkShaderModule(VK_NULL_HANDLE) {

}

VulkanShader::~VulkanShader() {
  destroy();
}

bool VulkanShader::load(StringRef filename, StringRef entry) {
  SPIRVCode code;
  return readSPIRV(code, filename) && load(code, entry);
}

bool VulkanShader::load(SPIRVCodeRef code, StringRef entry) {
  VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  mEntryFunction = entry;
  VkDevice device = mDevice->getVkDevice();
  return vkCreateShaderModule(device, &createInfo, nullptr, &mVkShaderModule) == VK_SUCCESS;
}

void VulkanShader::destroy() {
  mEntryFunction = "";
  if (mVkShaderModule != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyShaderModule(device, mVkShaderModule, nullptr);
    mVkShaderModule = VK_NULL_HANDLE;
  }
}

bool VulkanShader::readSPIRV(SPIRVCode& code, StringRef filename) {
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
