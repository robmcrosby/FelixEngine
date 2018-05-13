//
//  VulkanVertexMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanVertexMesh_h
#define VulkanVertexMesh_h

#include "GraphicResources.h"
#include <vulkan/vulkan.h>


namespace fx {
  
  /**
   *
   */
  class VulkanVertexMesh: public VertexMesh {
  private:
    VkVertexInputBindingDescription _bindingDescription;
    std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
    
    std::vector<VkBuffer> _vertexBuffers;
    std::vector<VkDeviceSize> _vertexOffsets;
    std::vector<VkDeviceMemory> _vertexMemory;
    
  public:
    VulkanVertexMesh();
    virtual ~VulkanVertexMesh();
    
    virtual bool load(const VertexMeshData &data);
    virtual void setPrimativeType(VERTEX_PRIMITIVE type);
    virtual bool setIndexBuffer(size_t count, const int *buffer);
    virtual bool setVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer);
    
    uint32_t getBindingDescriptionsCount() const {return 1;}
    VkVertexInputBindingDescription* getBindingDescriptions() {return &_bindingDescription;}
    
    uint32_t getAttributeDescriptionsCount() const {return (uint32_t)_attributeDescriptions.size();}
    VkVertexInputAttributeDescription* getAttributeDescriptions() {return _attributeDescriptions.data();}
    
    uint32_t getVertexBufferCount() {return (uint32_t)_vertexBuffers.size();}
    VkBuffer* getVertexBuffers() {return _vertexBuffers.data();}
    VkDeviceSize* getVertexOffsets() {return _vertexOffsets.data();}
    
    void clearBuffers();
    
  private:
    VkFormat getFloatFormatForSize(size_t size) const;
    uint32_t findMemoryType(uint32_t typeFilter, uint32_t properties) const;
  };
}

#endif /* VulkanVertexMesh_h */
