#include "VulkanIncludes.hpp"
#include <fstream>

using namespace std;


VulkanCommand::VulkanCommand(VulkanQueue* queue):
  mQueue(queue),
  mKernal(VK_NULL_HANDLE) {

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

void VulkanCommand::destroy() {
  clearShaders();
}

bool VulkanCommand::setKernal(StringRef filename, StringRef entry) {
  SPIRVCode code;
  return readSPIRV(code, filename) && setKernal(code, entry);
}

bool VulkanCommand::setKernal(SPIRVCodeRef code, StringRef entry) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  mKernalEntry = entry;
  VkDevice device = mQueue->getDevice()->getVkDevice();
  return vkCreateShaderModule(device, &createInfo, nullptr, &mKernal) == VK_SUCCESS;
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
