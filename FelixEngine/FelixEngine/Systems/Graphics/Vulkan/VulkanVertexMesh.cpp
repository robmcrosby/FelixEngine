//
//  VulkanVertexMesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanVertexMesh.h"
#include "VulkanShaderProgram.h"
#include "Vulkan.h"


using namespace std;
using namespace fx;

VulkanVertexMesh::VulkanVertexMesh() {
  _bindingDescription.binding = 0;
  _bindingDescription.stride = 0;
  _bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  
  _primitiveType = VERTEX_TRIANGLES;
  _meshData = nullptr;
  
  _totalVertices = 0;
}

VulkanVertexMesh::~VulkanVertexMesh() {
  clearMeshData();
  clearBuffers();
}

bool VulkanVertexMesh::load(const VertexMeshData &data) {
  uint32_t vertexCount = data.totalVertices;
  uint32_t vertexSize = 0;
  
  // Create the vertex attributes
  _vertexAttributes.clear();
  for (const auto &buffer : data.buffers) {
    size_t bufferSize = buffer.second.size();
    size_t componentSize = bufferSize/vertexCount;
    
    VertexAttribute attribute;
    attribute.name = buffer.first;
    attribute.offset = vertexSize * sizeof(float);
    attribute.format = Vulkan::getFloatFormatForSize(componentSize);
    _vertexAttributes.push_back(attribute);
    
    // Add to the vertex size
    vertexSize += componentSize;
  }
  
  // Set the stride of the vertex buffer binding
  _bindingDescription.stride = vertexSize * sizeof(float);
  
  // Create an inlined vertex buffer
  vector<float> vertices(vertexSize*vertexCount);
  uint32_t offset = 0;
  for (const auto &buffer : data.buffers) {
    size_t bufferSize = buffer.second.size();
    size_t componentSize = bufferSize/vertexCount;
    for (uint32_t i = 0; i < vertexCount; ++i) {
      float *dst = vertices.data() + (i*vertexSize + offset);
      const float *src = buffer.second.data() + (i*componentSize);
      memcpy(dst, src, componentSize*sizeof(float));
    }
    offset += componentSize;
  }
  
  _totalVertices = vertexCount;
  
  return createVertexBuffer(vertices);
}

vector<VkVertexInputAttributeDescription> VulkanVertexMesh::getAttributeDescriptions(VulkanShaderProgram *shader) {
  vector<VkVertexInputAttributeDescription> attributes;
  for (auto &attribute : _vertexAttributes) {
    int location = shader->getVertexLocation(attribute.name);
    if (location >= 0) {
      VkVertexInputAttributeDescription attributeDescription = {};
      attributeDescription.binding  = 0;
      attributeDescription.location = (uint32_t)location;
      attributeDescription.format   = attribute.format;
      attributeDescription.offset   = attribute.offset;
      attributes.push_back(attributeDescription);
    }
  }
  return attributes;
}

void VulkanVertexMesh::setPrimativeType(VERTEX_PRIMITIVE type) {
  _primitiveType = type;
}

bool VulkanVertexMesh::setIndexBuffer(size_t count, const int *buffer) {
  if (_meshData == nullptr)
    _meshData = new VertexMeshData();
  
  _meshData->indices.resize(count);
  memcpy(_meshData->indices.data(), buffer, count*sizeof(int));
  
  return true;
}

bool VulkanVertexMesh::setVertexBuffer(const string &name, size_t size, size_t count, const float *buffer) {
  // Create the Mesh Data and set total vertices
  if (_meshData == nullptr)
    _meshData = new VertexMeshData();
  if (_meshData->totalVertices == 0)
    _meshData->totalVertices = (int)count;
  
  // Copy the buffer to the Mesh Data
  _meshData->buffers[name].resize(size*count);
  memcpy(_meshData->buffers[name].data(), buffer, size*count*sizeof(float));
  
  return true;
}

bool VulkanVertexMesh::loadBuffers() {
  bool success = false;
  if (_meshData != nullptr) {
    _meshData->primitiveType = _primitiveType;
    success = load(*_meshData);
    if (success)
      clearMeshData();
  }
  return success;
}

void VulkanVertexMesh::drawToCommandBuffer(VkCommandBuffer commandBuffer) {
  
  
  vkCmdDraw(commandBuffer, _totalVertices, 1, 0, 0);
}

void VulkanVertexMesh::clearMeshData() {
  if (_meshData != nullptr) {
    delete _meshData;
    _meshData = nullptr;
  }
}

void VulkanVertexMesh::clearBuffers() {
  for (auto vertexBuffer : _vertexBuffers)
    vkDestroyBuffer(Vulkan::device, vertexBuffer, nullptr);
  _vertexBuffers.clear();
  
  for (auto vertexMemory : _vertexMemory)
    vkFreeMemory(Vulkan::device, vertexMemory, nullptr);
  _vertexMemory.clear();
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

bool VulkanVertexMesh::createVertexBuffer(const fx::VertexBuffer &vertices) {
  // Create the Vertex Buffer
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.pNext = nullptr;
  bufferInfo.size = vertices.size() * sizeof(float);
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
  void* deviceMemPtr;
  vkMapMemory(Vulkan::device, vertexBufferMemory, 0, bufferInfo.size, 0, &deviceMemPtr);
  memcpy(deviceMemPtr, vertices.data(), (size_t)bufferInfo.size);
  vkUnmapMemory(Vulkan::device, vertexBufferMemory);
  
  // Save the buffer and memory handles
  _vertexBuffers.push_back(vertexBuffer);
  _vertexOffsets.push_back(0);
  _vertexMemory.push_back(vertexBufferMemory);
  
  return true;
}
