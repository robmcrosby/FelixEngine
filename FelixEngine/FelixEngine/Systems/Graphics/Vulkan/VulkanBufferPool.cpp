//
//  VulkanBufferPool.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/19/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanBufferPool.h"
#include "Vulkan.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTextureBuffer.h"
#include "VulkanShaderProgram.h"


using namespace std;
using namespace fx;

VulkanBufferPool::VulkanBufferPool() {
  
}

VulkanBufferPool::~VulkanBufferPool() {
  clearDescriptorSets();
}

void VulkanBufferPool::addDescriptorSet(vector<VkDescriptorSet> &descriptorSets, VulkanShaderProgram *shader, UniformMap &map) {
  if (_descriptorSetMap.count(shader) == 0) {
    bool isVertex = isVertexUniform(shader, map);
    
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = (uint32_t)map.size();
    
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;
    poolInfo.flags = 0;
    
    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(Vulkan::device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
      cerr << "Failed to create descriptor pool" << endl;
      assert(false);
    }
    _descriptorPoolMap[shader] = descriptorPool;
    
    
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = isVertex ? shader->getVertexSetLayouts() : shader->getFragmentSetLayouts();
    
    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(Vulkan::device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
      cerr << "Failed to allocate descriptor set" << endl;
      assert(false);
    }
    _descriptorSetMap[shader] = descriptorSet;
    
    
    vector<VkDescriptorBufferInfo> bufferInfos;
    for (auto &uniform : map) {
      // Create any missing uniform buffers
      if (!uniform.second.usingBuffer())
        uniform.second.useBuffer();
      
      // Downcast to a VulkanUniformBuffer
      VulkanUniformBuffer *uniformBuffer = static_cast<VulkanUniformBuffer*>(uniform.second.buffer().get());
      
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = uniformBuffer->getVulkanBuffer();
      bufferInfo.offset = 0;
      bufferInfo.range = uniformBuffer->getVulkanBufferSize();
      
      bufferInfos.push_back(bufferInfo);
    }
    
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = (uint32_t)bufferInfos.size();
    descriptorWrite.pBufferInfo = bufferInfos.data();
    descriptorWrite.pImageInfo = nullptr; // Optional
    descriptorWrite.pTexelBufferView = nullptr; // Optional
    
    vkUpdateDescriptorSets(Vulkan::device, 1, &descriptorWrite, 0, nullptr);
    
  }
  descriptorSets.push_back(_descriptorSetMap.at(shader));
}

void VulkanBufferPool::addDescriptorSet(std::vector<VkDescriptorSet> &descriptorSets, VulkanShaderProgram *shader, TextureMap &map) {
  if (_descriptorSetMap.count(shader) == 0) {
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = (uint32_t)map.size();
    
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;
    poolInfo.flags = 0;
    
    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(Vulkan::device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
      cerr << "Failed to create descriptor pool" << endl;
      assert(false);
    }
    _descriptorPoolMap[shader] = descriptorPool;
    
    
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = shader->getFragmentSetLayouts();
    
    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(Vulkan::device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
      cerr << "Failed to allocate descriptor set" << endl;
      assert(false);
    }
    _descriptorSetMap[shader] = descriptorSet;
    
    
    vector<VkDescriptorImageInfo> imageInfos;
    for (auto &texture : map) {
      // Downcast to a VulkanTextureBuffer
      VulkanTextureBuffer *textureBuffer = static_cast<VulkanTextureBuffer*>(texture.buffer.get());
      
      VkDescriptorImageInfo imageInfo = {};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = textureBuffer->getImageView();
      imageInfo.sampler = VulkanTextureBuffer::getSamplerForState(texture.sampler);
      
      imageInfos.push_back(imageInfo);
    }
    
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = (uint32_t)imageInfos.size();
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = imageInfos.data();
    descriptorWrite.pTexelBufferView = nullptr; // Optional
    
    vkUpdateDescriptorSets(Vulkan::device, 1, &descriptorWrite, 0, nullptr);
    
  }
  descriptorSets.push_back(_descriptorSetMap.at(shader));
}

void VulkanBufferPool::addDescriptorSet(std::vector<VkDescriptorSet> &descriptorSets, VulkanShaderProgram *shader, TextureMap &textures, UniformMap &uniforms) {
  if (_descriptorSetMap.count(shader) == 0) {
    VkDescriptorPoolSize poolSizes[] = {{}, {}};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = (uint32_t)uniforms.size();
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = (uint32_t)textures.size();
    
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = 1;
    poolInfo.flags = 0;
    
    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(Vulkan::device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
      cerr << "Failed to create descriptor pool" << endl;
      assert(false);
    }
    _descriptorPoolMap[shader] = descriptorPool;
    
    
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = shader->getFragmentSetLayouts();
    
    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(Vulkan::device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
      cerr << "Failed to allocate descriptor set" << endl;
      assert(false);
    }
    _descriptorSetMap[shader] = descriptorSet;
    
    
    vector<VkDescriptorBufferInfo> bufferInfos;
    for (auto &uniform : uniforms) {
      // Create any missing uniform buffers
      if (!uniform.second.usingBuffer())
      uniform.second.useBuffer();
      
      // Downcast to a VulkanUniformBuffer
      VulkanUniformBuffer *uniformBuffer = static_cast<VulkanUniformBuffer*>(uniform.second.buffer().get());
      
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = uniformBuffer->getVulkanBuffer();
      bufferInfo.offset = 0;
      bufferInfo.range = uniformBuffer->getVulkanBufferSize();
      
      bufferInfos.push_back(bufferInfo);
    }
    
    
    vector<VkDescriptorImageInfo> imageInfos;
    for (auto &texture : textures) {
      // Downcast to a VulkanTextureBuffer
      VulkanTextureBuffer *textureBuffer = static_cast<VulkanTextureBuffer*>(texture.buffer.get());
      
      VkDescriptorImageInfo imageInfo = {};
      imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfo.imageView = textureBuffer->getImageView();
      imageInfo.sampler = VulkanTextureBuffer::getSamplerForState(texture.sampler);
      
      imageInfos.push_back(imageInfo);
    }
    
    
    VkWriteDescriptorSet descriptorWrites[] = {{}, {}};
    
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = (uint32_t)bufferInfos.size();
    descriptorWrites[0].pBufferInfo = bufferInfos.data();
    
    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = (uint32_t)imageInfos.size();
    descriptorWrites[1].pImageInfo = imageInfos.data();
    
    vkUpdateDescriptorSets(Vulkan::device, 2, descriptorWrites, 0, nullptr);
  }
  descriptorSets.push_back(_descriptorSetMap.at(shader));
}

void VulkanBufferPool::clearDescriptorSets() {
  for (auto &pool : _descriptorPoolMap)
    vkDestroyDescriptorPool(Vulkan::device, pool.second, nullptr);
  _descriptorPoolMap.clear();
  _descriptorSetMap.clear();
}

bool VulkanBufferPool::isVertexUniform(VulkanShaderProgram *shader, UniformMap &map) {
  for (auto &uniform : map) {
    if (shader->getVertexUniformBinding(uniform.first) >= 0)
      return true;
  }
  return false;
}
