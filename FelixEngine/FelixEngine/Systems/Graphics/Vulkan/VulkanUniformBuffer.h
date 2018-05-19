//
//  VulkanUniformBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanUniformBuffer_h
#define VulkanUniformBuffer_h

#include "GraphicResources.h"
#include <vulkan/vulkan.h>


namespace fx {
  class VulkanShaderProgram;
  typedef std::map<VulkanShaderProgram*, VkDescriptorSet> DescriptorSetMap;
  
  /**
   *
   */
  class VulkanUniformBuffer: public UniformBuffer {
  private:
    size_t _size;
    
    VkBuffer _uniformBuffer;
    VkDeviceMemory _uniformBufferMemory;
    
    VkDescriptorPool _descriptorPool;
    DescriptorSetMap _descriptorSetMap;
    
  public:
    VulkanUniformBuffer();
    virtual ~VulkanUniformBuffer();
    
    virtual bool load(void *data, size_t size, BUFFER_COUNT count);
    virtual void update(void *data, size_t size);
    
    VkDescriptorSet getDescriptorSet(VulkanShaderProgram *shader);
    
  private:
    bool createUniformBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    uint32_t findMemoryType(uint32_t typeFilter, uint32_t properties) const;
  };
}

#endif /* VulkanUniformBuffer_h */
