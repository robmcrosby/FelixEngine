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
#include "VertexMeshData.h"
#include <vulkan/vulkan.h>


namespace fx {
  class VulkanShaderProgram;
  
  struct VertexAttribute {
    std::string name;
    uint32_t offset;
    VkFormat format;
  };
  
  /**
   *
   */
  class VulkanVertexMesh: public VertexMesh {
  private:
    VkVertexInputBindingDescription _bindingDescription;
    std::vector<VertexAttribute> _vertexAttributes;
    
    std::vector<VkBuffer> _vertexBuffers;
    std::vector<VkDeviceSize> _vertexOffsets;
    std::vector<VkDeviceMemory> _vertexMemory;
    
    uint32_t _totalVertices;
    
    VERTEX_PRIMITIVE _primitiveType;
    VertexMeshData *_meshData;
    
  public:
    VulkanVertexMesh();
    virtual ~VulkanVertexMesh();
    
    virtual bool load(const VertexMeshData &data);
    virtual void setPrimativeType(VERTEX_PRIMITIVE type);
    virtual bool setIndexBuffer(size_t count, const int *buffer);
    virtual bool setVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer);
    virtual bool loadBuffers();
    
    uint32_t getBindingDescriptionsCount() const {return 1;}
    VkVertexInputBindingDescription* getBindingDescriptions() {return &_bindingDescription;}
    
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(VulkanShaderProgram *shader);
    
    uint32_t getVertexBufferCount() {return (uint32_t)_vertexBuffers.size();}
    VkBuffer* getVertexBuffers() {return _vertexBuffers.data();}
    VkDeviceSize* getVertexOffsets() {return _vertexOffsets.data();}
    
    void drawToCommandBuffer(VkCommandBuffer commandBuffer);
    
    void clearMeshData();
    void clearBuffers();
    
  private:
    uint32_t findMemoryType(uint32_t typeFilter, uint32_t properties) const;
    bool createVertexBuffer(const fx::VertexBuffer &vertices);
  };
}

#endif /* VulkanVertexMesh_h */
