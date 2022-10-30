#include "VulkanIncludes.hpp"
#include "spirv_reflect.h"
#include <fstream>


using namespace std;

VulkanShader::VulkanShader(VulkanDevice* device, VkShaderStageFlagBits stage):
  mDevice(device),
  mVkShaderModule(VK_NULL_HANDLE),
  mVkShaderStage(stage) {

}

VulkanShader::~VulkanShader() {
  destroy();
}

bool VulkanShader::load(StringRef filename, StringRef entry) {
  SPIRVCode code;
  return readSPIRV(code, filename) && load(code, entry);
}

bool VulkanShader::load(SPIRVCodeRef code, StringRef entry) {
  reflect(code, entry);

  VkShaderModuleCreateInfo createInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
  createInfo.codeSize = code.size() * sizeof(uint32_t);
  createInfo.pCode = code.data();

  mEntryFunction = entry;
  VkDevice device = mDevice->getVkDevice();
  return vkCreateShaderModule(device, &createInfo, nullptr, &mVkShaderModule) == VK_SUCCESS;
}

VkPipelineShaderStageCreateInfo VulkanShader::getVkPipelineShaderStageCreateInfo() {
  VkPipelineShaderStageCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, 0, 0};
  createInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  createInfo.pNext  = nullptr;
  createInfo.flags  = 0;
  createInfo.stage  = mVkShaderStage;
  createInfo.module = mVkShaderModule;
  createInfo.pName  = mEntryFunction.c_str();
  createInfo.pSpecializationInfo = nullptr;
  return createInfo;
}

void VulkanShader::destroy() {
  mEntryFunction = "";
  if (mVkShaderModule != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyShaderModule(device, mVkShaderModule, nullptr);
    mVkShaderModule = VK_NULL_HANDLE;
  }
}

void VulkanShader::reflect(SPIRVCodeRef code, StringRef entry) {
  mLayoutBindingSets.clear();

  SpvReflectShaderModule module;
  size_t size = code.size() * sizeof(uint32_t);
  if (spvReflectCreateShaderModule(size, code.data(), &module) == SPV_REFLECT_RESULT_SUCCESS) {
    uint32_t count;
    if (spvReflectEnumerateEntryPointDescriptorSets(&module, entry.c_str(), &count, NULL) == SPV_REFLECT_RESULT_SUCCESS) {
      vector<SpvReflectDescriptorSet*> descriptorSets(count);
      spvReflectEnumerateEntryPointDescriptorSets(&module, entry.c_str(), &count, descriptorSets.data());

      mLayoutBindingSets.resize(count);
      for (auto set : descriptorSets) {
        auto& layoutBindings = mLayoutBindingSets.at(set->set);
        layoutBindings.resize(set->binding_count);
        for (int i = 0; i < set->binding_count; ++i) {
          SpvReflectDescriptorBinding* binding = set->bindings[i];
          auto& layout = layoutBindings.at(i);
          layout.name = binding->name;
          layout.binding.binding = binding->binding;
          layout.binding.descriptorType = (VkDescriptorType)binding->descriptor_type;
          layout.binding.descriptorCount = binding->count;
          layout.binding.stageFlags = mVkShaderStage;
          layout.binding.pImmutableSamplers = nullptr;
        }
      }
    }
    spvReflectDestroyShaderModule(&module);
  }
  else {
    cerr << "Error reflecting shader source" << endl;
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
