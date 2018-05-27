//
//  VulkanUniformBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanUniformBuffer.h"
#include "VulkanShaderProgram.h"
#include "Vulkan.h"


using namespace std;
using namespace fx;

VulkanUniformBuffer::VulkanUniformBuffer() {
  _size = 0;
  
  _uniformBuffer = VK_NULL_HANDLE;
  _uniformBufferMemory = VK_NULL_HANDLE;
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
  if (_uniformBuffer != VK_NULL_HANDLE)
    vkDestroyBuffer(Vulkan::device, _uniformBuffer, nullptr);
  if (_uniformBufferMemory != VK_NULL_HANDLE)
    vkFreeMemory(Vulkan::device, _uniformBufferMemory, nullptr);
}

bool VulkanUniformBuffer::load(void *data, size_t size, BUFFER_COUNT count) {
  if (!createUniformBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
    return false;
  
  Vulkan::upload(_uniformBufferMemory, data, size);
  _size = size;
  
  return true;
}

void VulkanUniformBuffer::update(void *data, size_t size) {
  if (_uniformBufferMemory != VK_NULL_HANDLE) {
    Vulkan::upload(_uniformBufferMemory, data, size);
    _size = size;
  }
}

bool VulkanUniformBuffer::createUniformBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
  _uniformBuffer = Vulkan::createBuffer(size, usage);
  _uniformBufferMemory = Vulkan::allocateMemory(_uniformBuffer, properties);
  return _uniformBuffer != VK_NULL_HANDLE && _uniformBufferMemory != VK_NULL_HANDLE;
}
