//
//  VulkanCommandPool.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/11/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanCommandPool_h
#define VulkanCommandPool_h

#include "VulkanGraphics.h"


namespace fx {
  class VulkanCommandBuffer;
  
  /**
   *
   */
  class VulkanCommandPool {
  private:
    std::vector<VulkanCommandBuffer*> _commandBuffers;
    
  public:
    VulkanCommandPool();
    ~VulkanCommandPool();
    
    void submitRenderPasses(std::vector<RenderPass> &passes);
    
  private:
    void resizeCommandBuffers(size_t size);
    void clearCommandBuffers();
  };
}

#endif /* VulkanCommandPool_h */
