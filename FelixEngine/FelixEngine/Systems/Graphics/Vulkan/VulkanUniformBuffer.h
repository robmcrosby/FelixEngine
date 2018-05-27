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
  /**
   *
   */
  class VulkanUniformBuffer: public UniformBuffer {
  private:
    size_t _size;
    
    VkBuffer _uniformBuffer;
    VkDeviceMemory _uniformBufferMemory;
    
  public:
    VulkanUniformBuffer();
    virtual ~VulkanUniformBuffer();
    
    virtual bool load(void *data, size_t size, BUFFER_COUNT count);
    virtual void update(void *data, size_t size);
    
    VkBuffer getVulkanBuffer() {return _uniformBuffer;}
    uint32_t getVulkanBufferSize() {return (uint32_t)_size;}
    
  private:
    bool createUniformBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
  };
}

#endif /* VulkanUniformBuffer_h */
