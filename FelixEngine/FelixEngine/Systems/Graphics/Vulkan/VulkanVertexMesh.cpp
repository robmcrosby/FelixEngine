//
//  VulkanVertexMesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanVertexMesh.h"
#include "Vulkan.h"


using namespace std;
using namespace fx;

VulkanVertexMesh::VulkanVertexMesh() {
  _bindingDescription.binding = 0;
  _bindingDescription.stride = 0;
  _bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

VulkanVertexMesh::~VulkanVertexMesh() {
  clearBuffers();
}

bool VulkanVertexMesh::load(const VertexMeshData &data) {
  return true;
}

void VulkanVertexMesh::setPrimativeType(VERTEX_PRIMITIVE type) {
  
}

bool VulkanVertexMesh::setIndexBuffer(size_t count, const int *buffer) {
  return true;
}

bool VulkanVertexMesh::setVertexBuffer(const string &name, size_t size, size_t count, const float *buffer) {
  // Add an Attribute
  VkVertexInputAttributeDescription attributeDescription = {};
  attributeDescription.binding = 0;
  attributeDescription.location = 0; // TODO: Implement Shader Reflection to find locations for names
  attributeDescription.format = getFloatFormatForSize(size);
  attributeDescription.offset = _bindingDescription.stride;
  _attributeDescriptions.push_back(attributeDescription);
  
  // Update the binding stride
  _bindingDescription.stride += (uint32_t)size * sizeof(float);
  
  
  
  // TODO: Add this as a final create step
  // Create the Vertex Buffer
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.pNext = nullptr;
  bufferInfo.size = size * count * sizeof(float);
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferInfo.queueFamilyIndexCount = 0;
  bufferInfo.pQueueFamilyIndices = nullptr;
  bufferInfo.flags = 0;

  VkBuffer vertexBuffer;
  if (vkCreateBuffer(Vulkan::device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
    cerr << "failed to create vertex buffer" << endl;
    return false;
  }
  
  // Get Memory Requirements
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(Vulkan::device, vertexBuffer, &memRequirements);
  
  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  // Allocate Buffer Memory
  VkDeviceMemory vertexBufferMemory;
  if (vkAllocateMemory(Vulkan::device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
    cerr << "failed to allocate vertex buffer memory!";
    return false;
  }
  
  // Bind the Vertex Buffer with the Buffer Memory
  vkBindBufferMemory(Vulkan::device, vertexBuffer, vertexBufferMemory, 0);
  
  // Copy the Vertex Data to the Vertex Buffer
  void* data;
  vkMapMemory(Vulkan::device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
  memcpy(data, buffer, (size_t)bufferInfo.size);
  vkUnmapMemory(Vulkan::device, vertexBufferMemory);
  
  // Save the buffer and memory handles
  _vertexBuffers.push_back(vertexBuffer);
  _vertexOffsets.push_back(0);
  _vertexMemory.push_back(vertexBufferMemory);
  
  return true;
}

void VulkanVertexMesh::clearBuffers() {
  for (auto vertexBuffer : _vertexBuffers)
    vkDestroyBuffer(Vulkan::device, vertexBuffer, nullptr);
  _vertexBuffers.clear();
  
  for (auto vertexMemory : _vertexMemory)
    vkFreeMemory(Vulkan::device, vertexMemory, nullptr);
  _vertexMemory.clear();
}

VkFormat VulkanVertexMesh::getFloatFormatForSize(size_t size) const {
  switch (size) {
    case 1:
      return VK_FORMAT_R32_SFLOAT;
    case 2:
      return VK_FORMAT_R32G32_SFLOAT;
    case 3:
      return VK_FORMAT_R32G32B32_SFLOAT;
    case 4:
      return VK_FORMAT_R32G32B32A32_SFLOAT;
    default:
      cerr << "No Float Formats of size: " << size << endl;
      return VK_FORMAT_UNDEFINED;
  }
}

uint32_t VulkanVertexMesh::findMemoryType(uint32_t typeFilter, uint32_t properties) const {
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
