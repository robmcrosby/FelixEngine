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


namespace fx {
  
  /**
   *
   */
  class VulkanUniformBuffer: public UniformBuffer {
  private:
    
  public:
    VulkanUniformBuffer();
    virtual ~VulkanUniformBuffer();
    
    virtual bool load(void *data, size_t size, BUFFER_COUNT count);
    virtual void update(void *data, size_t size);
  };
}

#endif /* VulkanUniformBuffer_h */
