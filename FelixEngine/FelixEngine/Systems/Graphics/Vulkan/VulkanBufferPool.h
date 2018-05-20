//
//  VulkanBufferPool.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/19/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanBufferPool_h
#define VulkanBufferPool_h

#include "GraphicResources.h"
#include "UniformMap.h"
#include <vulkan/vulkan.h>


namespace fx {
  class VulkanShaderProgram;
  typedef std::map<VulkanShaderProgram*, VkDescriptorPool> DescriptorPoolMap;
  typedef std::map<VulkanShaderProgram*, VkDescriptorSet>  DescriptorSetMap;
  
  /**
   *
   */
  class VulkanBufferPool: public BufferPool {
  private:
    DescriptorPoolMap _descriptorPoolMap;
    DescriptorSetMap  _descriptorSetMap;
    
  public:
    VulkanBufferPool();
    virtual ~VulkanBufferPool();
    
    void addDescriptorSet(std::vector<VkDescriptorSet> &descriptorSets, VulkanShaderProgram *shader, UniformMap &map);
    
  private:
    void clearDescriptorSets();
  };
}

#endif /* VulkanBufferPool_h */
