//
//  VulkanCommandBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/11/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanCommandBuffer_h
#define VulkanCommandBuffer_h

#include "VulkanGraphics.h"
#include "GraphicResources.h"
#include <vulkan/vulkan.h>


namespace fx {
  
  /**
   *
   */
  class VulkanCommandBuffer {
  private:
    std::vector<VkCommandBuffer> _commandBuffers;
    
  public:
    VulkanCommandBuffer();
    ~VulkanCommandBuffer();
    
    void submitRenderPass(RenderPass &pass);
    
  private:
    void resizeBuffers(size_t size);
    void clearBuffers();
  };
}

#endif /* VulkanCommandBuffer_h */
