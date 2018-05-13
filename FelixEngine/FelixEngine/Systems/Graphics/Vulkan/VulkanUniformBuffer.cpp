//
//  VulkanUniformBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanUniformBuffer.h"
#include "Vulkan.h"


using namespace std;
using namespace fx;

VulkanUniformBuffer::VulkanUniformBuffer() {
  _descriptorSetLayout = VK_NULL_HANDLE;
  
  _uniformBuffer = VK_NULL_HANDLE;
  _uniformBufferMemory = VK_NULL_HANDLE;
  
  _descriptorPool = VK_NULL_HANDLE;
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
  if (_descriptorSetLayout != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(Vulkan::device, _descriptorSetLayout, nullptr);
  
  if (_uniformBuffer != VK_NULL_HANDLE)
    vkDestroyBuffer(Vulkan::device, _uniformBuffer, nullptr);
  if (_uniformBufferMemory != VK_NULL_HANDLE)
    vkFreeMemory(Vulkan::device, _uniformBufferMemory, nullptr);
  
  if (_descriptorPool != VK_NULL_HANDLE)
    vkDestroyDescriptorPool(Vulkan::device, _descriptorPool, nullptr);
}

bool VulkanUniformBuffer::load(void *data, size_t size, BUFFER_COUNT count) {
  
  VkDescriptorSetLayoutBinding layoutBinding = {};
  layoutBinding.binding = 0;
  layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layoutBinding.descriptorCount = 1;
  layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBinding.pImmutableSamplers = nullptr; // Optional
  
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &layoutBinding;
  
  if (vkCreateDescriptorSetLayout(Vulkan::device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
    cerr << "Failed to create descriptor set layout" << endl;
    return false;
  }
  
  
  if (!createUniformBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
    return false;
  
  void* deviceMemPtr;
  vkMapMemory(Vulkan::device, _uniformBufferMemory, 0, size, 0, &deviceMemPtr);
  memcpy(deviceMemPtr, data, size);
  vkUnmapMemory(Vulkan::device, _uniformBufferMemory);
  
  
  VkDescriptorPoolSize poolSize = {};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = 1;
  
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = 1;
  poolInfo.flags = 0;
  
  if (vkCreateDescriptorPool(Vulkan::device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
    cerr << "Failed to create descriptor pool" << endl;
    return false;
  }
  
  
  VkDescriptorSetLayout layouts[] = {_descriptorSetLayout};
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = _descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = layouts;
  
  if (vkAllocateDescriptorSets(Vulkan::device, &allocInfo, &_descriptorSet) != VK_SUCCESS) {
    cerr << "Failed to allocate descriptor set" << endl;
    return false;
  }
  
  
  VkDescriptorBufferInfo bufferInfo = {};
  bufferInfo.buffer = _uniformBuffer;
  bufferInfo.offset = 0;
  bufferInfo.range = size;
  
  VkWriteDescriptorSet descriptorWrite = {};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = _descriptorSet;
  descriptorWrite.dstBinding = 0;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo = &bufferInfo;
  descriptorWrite.pImageInfo = nullptr; // Optional
  descriptorWrite.pTexelBufferView = nullptr; // Optional
  
  vkUpdateDescriptorSets(Vulkan::device, 1, &descriptorWrite, 0, nullptr);
  
  return true;
}

void VulkanUniformBuffer::update(void *data, size_t size) {
  if (_uniformBufferMemory != VK_NULL_HANDLE) {
    void* deviceMemPtr;
    vkMapMemory(Vulkan::device, _uniformBufferMemory, 0, size, 0, &deviceMemPtr);
    memcpy(deviceMemPtr, data, size);
    vkUnmapMemory(Vulkan::device, _uniformBufferMemory);
  }
}

bool VulkanUniformBuffer::createUniformBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  
  if (vkCreateBuffer(Vulkan::device, &bufferInfo, nullptr, &_uniformBuffer) != VK_SUCCESS) {
    cerr << "Failed to create uniform buffer" << endl;
    return false;
  }
  
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(Vulkan::device, _uniformBuffer, &memRequirements);
  
  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
  
  if (vkAllocateMemory(Vulkan::device, &allocInfo, nullptr, &_uniformBufferMemory) != VK_SUCCESS) {
    cerr << "failed to allocate buffer memory!" << endl;
  }
  
  vkBindBufferMemory(Vulkan::device, _uniformBuffer, _uniformBufferMemory, 0);
  return true;
}

uint32_t VulkanUniformBuffer::findMemoryType(uint32_t typeFilter, uint32_t properties) const {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(Vulkan::physicalDevice, &memProperties);
  
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    uint32_t flags = (uint32_t)memProperties.memoryTypes[i].propertyFlags;
    if ((typeFilter & (1 << i)) && (flags & properties) == properties) {
      return i;
    }
  }
  
  cerr << "Failed to find suitable memory type" << endl;
  assert(false);
  return 0;
}
