//
//  VulkanShaderProgram.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanShaderProgram.h"
#include "Vulkan.h"
#include <sstream>

#include "spirv_cross.hpp"


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
  _vertexInputs.clear();
  _vertexUniforms.clear();
  _fragmentUniforms.clear();
  
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
  
  loadShaderModule(getShaderFileName(vertex), SHADER_VERTEX);
  loadShaderModule(getShaderFileName(fragment), SHADER_FRAGMENT);
  
  return _shaderStages[0].module != VK_NULL_HANDLE && _shaderStages[1].module != VK_NULL_HANDLE;
}

string VulkanShaderProgram::getShaderFileName(const string &name) const {
  stringstream ss;
  ss << "Shaders/" << name << ".spv";
  return ss.str();
}

bool VulkanShaderProgram::loadShaderModule(const string &fileName, SHADER_PART part) {
  FileData code;
  if (!FileSystem::loadData(code, fileName))
    return false;
  
  reflectShaderCode(code, part);
  
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
  
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(Vulkan::device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    cerr << "Error Creating Shader Module" << endl;
    return false;
  }
  _shaderStages[part].module = shaderModule;
  return true;
}

void VulkanShaderProgram::reflectShaderCode(FileData &code, SHADER_PART part) {
  size_t words = (size_t)ceil(code.size()/4.0);
  spirv_cross::Compiler compiler((uint32_t*)code.data(), words);
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();
  
  if (part == SHADER_VERTEX) {
    // Get the Vertex input info
    for (auto &resource : resources.stage_inputs) {
      VertexInput input;
      spirv_cross::SPIRType type = compiler.get_type(resource.type_id);
      input.name = resource.name;
      input.index = compiler.get_decoration(resource.id, spv::DecorationLocation);
      input.format = Vulkan::getFloatFormatForSize(type.vecsize);
      _vertexInputs.push_back(input);
    }
    
    // Get the Vertex Uniform info
    for (auto &resource: resources.uniform_buffers) {
      UniformInput input;
      input.name = resource.name;
      input.index = compiler.get_decoration(resource.id, spv::DecorationBinding);
      _vertexUniforms.push_back(input);
    }
    
    // TODO: get the Vertex Texture info
  }
  else {
    // Get the Fragment uniform info
    for (auto &resource: resources.uniform_buffers) {
      UniformInput input;
      input.name = resource.name;
      input.index = compiler.get_decoration(resource.id, spv::DecorationBinding);
      _fragmentUniforms.push_back(input);
    }
    
    // TODO: Get the Fragment textures info
  }
}
