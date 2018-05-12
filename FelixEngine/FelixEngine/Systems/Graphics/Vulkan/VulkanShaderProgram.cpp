//
//  VulkanShaderProgram.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanShaderProgram.h"
#include "Vulkan.h"
#include "FileSystem.h"
#include <sstream>


using namespace std;
using namespace fx;

VulkanShaderProgram::VulkanShaderProgram() {
  _shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  _shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  _shaderStages[0].module = VK_NULL_HANDLE;
  _shaderStages[0].pName = "main";
  
  _shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  _shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  _shaderStages[1].module = VK_NULL_HANDLE;
  _shaderStages[1].pName = "main";
}

VulkanShaderProgram::~VulkanShaderProgram() {
  clearShaderModules();
}

void VulkanShaderProgram::clearShaderModules() {
  if (_shaderStages[0].module != VK_NULL_HANDLE) {
    vkDestroyShaderModule(Vulkan::device, _shaderStages[0].module, nullptr);
    _shaderStages[0].module = VK_NULL_HANDLE;
  }
  if (_shaderStages[1].module != VK_NULL_HANDLE) {
    vkDestroyShaderModule(Vulkan::device, _shaderStages[1].module, nullptr);
    _shaderStages[1].module = VK_NULL_HANDLE;
  }
}

bool VulkanShaderProgram::loadShaderFunctions(const std::string &vertex, const std::string &fragment) {
  clearShaderModules();
  
  _shaderStages[0].module = loadShaderModule(getShaderFileName(vertex));
  _shaderStages[1].module = loadShaderModule(getShaderFileName(fragment));
  
  return _shaderStages[0].module != VK_NULL_HANDLE && _shaderStages[1].module != VK_NULL_HANDLE;
}

string VulkanShaderProgram::getShaderFileName(const string &name) const {
  stringstream ss;
  ss << "Shaders/" << name << ".spv";
  return ss.str();
}

VkShaderModule VulkanShaderProgram::loadShaderModule(const string &fileName) const {
  FileData code;
  if (!FileSystem::loadData(code, fileName))
    return VK_NULL_HANDLE;
  
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
  
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(Vulkan::device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    cerr << "Error Creating Shader Module" << endl;
    return VK_NULL_HANDLE;
  }
  return shaderModule;
}
