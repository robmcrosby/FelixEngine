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

unsigned int VulkanVertexMesh::meshCount = 0;

VulkanVertexMesh::VulkanVertexMesh() {
  _bindingDescription.binding = 0;
  _bindingDescription.stride = 0;
  _bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  
  _primitiveType = VERTEX_TRIANGLES;
  _meshData = nullptr;
  
  _totalVertices = 0;
  
  _meshId = meshCount++;
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

VkPrimitiveTopology VulkanVertexMesh::getPrimitiveTopology() const {
  return _primitiveType == VERTEX_TRIANGLES ? VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
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

unsigned int VulkanVertexMesh::getMeshId() const {
  return _meshId;
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

bool VulkanVertexMesh::createVertexBuffer(const fx::VertexBuffer &vertices) {
  // Create the Vertex Buffer
  VkDeviceSize size = vertices.size() * sizeof(float);
  VkBuffer vertexBuffer = Vulkan::createBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  
  // Allocate the Vertex Buffer Memory
  VkDeviceMemory bufferMemory = Vulkan::allocateMemory(vertexBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  // Copy the Vertex Data to the Vertex Buffer
  Vulkan::upload(bufferMemory, vertices.data(), size);
  
  // Save the buffer and memory handles
  _vertexBuffers.push_back(vertexBuffer);
  _vertexOffsets.push_back(0);
  _vertexMemory.push_back(bufferMemory);
  
  return true;
}
