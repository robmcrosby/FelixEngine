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
  _totalVertexUniformSets = 0;
  _totalFragmentUniformSets = 0;
  _totalFragmentTextureSets = 0;
  
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
  _totalVertexUniformSets = 0;
  _totalFragmentUniformSets = 0;
  _totalFragmentTextureSets = 0;
  
  _vertexUniforms.clear();
  _fragmentUniforms.clear();
  _fragmentTextures.clear();
  
  for (auto &layout : _vertexSetLayouts)
    vkDestroyDescriptorSetLayout(Vulkan::device, layout, nullptr);
  _vertexSetLayouts.clear();
  
  for (auto &layout : _fragmentSetLayouts)
    vkDestroyDescriptorSetLayout(Vulkan::device, layout, nullptr);
  _fragmentSetLayouts.clear();
  
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
  
  return _shaderStages[0].module != VK_NULL_HANDLE && _shaderStages[1].module != VK_NULL_HANDLE && createDescriptorSetLayouts();
}

int VulkanShaderProgram::getVertexLocation(const std::string &name) const {
  if (_vertexLocations.count(name) > 0)
    return _vertexLocations.at(name);
  return -1;
}

int VulkanShaderProgram::getVertexUniformBinding(const std::string &name) const {
  for (const auto &uniform : _vertexUniforms) {
    if (name == uniform.name)
      return uniform.binding;
  }
  return -1;
}

int VulkanShaderProgram::getFragmentUniformBinding(const std::string &name) const {
  for (const auto &uniform : _fragmentUniforms) {
    if (name == uniform.name)
      return uniform.binding;
  }
  return -1;
}

VkDescriptorSetLayout* VulkanShaderProgram::getVertexSetLayouts() {
  return _vertexSetLayouts.size() == 0 ? nullptr : _vertexSetLayouts.data();
}

VkDescriptorSetLayout* VulkanShaderProgram::getFragmentSetLayouts() {
  return _fragmentSetLayouts.size() == 0 ? nullptr : _fragmentSetLayouts.data();
}

vector<VkDescriptorSetLayout> VulkanShaderProgram::getDescriptorSetLayouts() const {
  vector<VkDescriptorSetLayout> combined = _vertexSetLayouts;
  combined.insert(combined.end(), _fragmentSetLayouts.begin(), _fragmentSetLayouts.end());
  return combined;
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
    // Get the Vertex Locations
    for (auto &resource : resources.stage_inputs) {
      int location = (int)compiler.get_decoration(resource.id, spv::DecorationLocation);
      _vertexLocations[resource.name] = location;
    }
    
    // Get the Vertex Uniform Bindings
    for (auto &resource : resources.uniform_buffers) {
      UniformInput input;
      input.name    = resource.name;
      input.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
      input.set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
      _vertexUniforms.push_back(input);
      _totalVertexUniformSets = _totalVertexUniformSets > input.set+1 ? _totalVertexUniformSets : input.set+1;
    }
    
    // TODO: Get the Vertex Texture Bindings
  }
  else {
    // Get the Fragment Uniform Bindings
    for (auto &resource : resources.uniform_buffers) {
      UniformInput input;
      input.name    = resource.name;
      input.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
      input.set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
      _fragmentUniforms.push_back(input);
      _totalFragmentUniformSets = _totalFragmentUniformSets > input.set+1 ? _totalFragmentUniformSets : input.set+1;
    }
    
    // Get the Fragment Textures Bindings
    for (auto &resource : resources.sampled_images) {
      TextureInput input;
      input.name    = resource.name;
      input.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
      input.set     = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
      _fragmentTextures.push_back(input);
      _totalFragmentTextureSets = _totalFragmentTextureSets > input.set+1 ? _totalFragmentTextureSets : input.set+1;
    }
  }
}

bool VulkanShaderProgram::createDescriptorSetLayouts() {
  vector<vector<VkDescriptorSetLayoutBinding> > vertexBindingSets(_totalVertexUniformSets);
  for (auto &uniform : _vertexUniforms) {
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = uniform.binding;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    binding.pImmutableSamplers = nullptr; // Optional
    vertexBindingSets[uniform.set].push_back(binding);
  }
  for (auto &bindingSet : vertexBindingSets) {
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = (uint32_t)bindingSet.size();
    layoutInfo.pBindings = bindingSet.size() == 0 ? nullptr : bindingSet.data();
    
    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(Vulkan::device, &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
      cerr << "Failed to create descriptor set layout" << endl;
      return false;
    }
    _vertexSetLayouts.push_back(layout);
  }
  
  int totalFramgnetSets = _totalFragmentUniformSets+_totalFragmentTextureSets;
  vector<vector<VkDescriptorSetLayoutBinding> > fragmentBindingSets(totalFramgnetSets);
  for (auto &uniform : _fragmentUniforms) {
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = uniform.binding;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.pImmutableSamplers = nullptr; // Optional
    fragmentBindingSets[uniform.set].push_back(binding);
  }
  for (auto &texture : _fragmentTextures) {
    VkDescriptorSetLayoutBinding binding = {};
    binding.binding = texture.binding;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    binding.pImmutableSamplers = nullptr; // Optional
    fragmentBindingSets[texture.set].push_back(binding);
  }
  for (auto &bindingSet : fragmentBindingSets) {
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = (uint32_t)bindingSet.size();
    layoutInfo.pBindings = bindingSet.size() == 0 ? nullptr : bindingSet.data();
    
    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(Vulkan::device, &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
      cerr << "Failed to create descriptor set layout" << endl;
      return false;
    }
    _fragmentSetLayouts.push_back(layout);
  }
  
  return true;
}
