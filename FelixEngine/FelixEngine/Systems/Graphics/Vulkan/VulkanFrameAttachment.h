//
//  VulkanFrameAttachment.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/8/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanFrameAttachment_h
#define VulkanFrameAttachment_h

#include "VulkanTextureBuffer.h"
#include "Vulkan.h"


namespace fx {
  
  /**
   *
   */
  class VulkanFrameAttachment {
  private:
    VulkanTextureBuffers _textureBuffers;
    std::vector<VkFramebuffer> _frameBuffers;
    bool _isSwapChain;
    
  public:
    VulkanFrameAttachment();
    ~VulkanFrameAttachment();
    
    bool loadSwapBuffers(int index = 0);
    bool loadDepthBuffer(VkFormat format, int width, int height);
    
    VkFormat getFormat() const;
    VkImageView getImageView(int index = 0) const;
    ivec2 getBufferSize() const;
    int getBufferCount() const {return (int)_textureBuffers.size();}
    
    bool isSwapChain() const {return _isSwapChain;}
    int getCurrentIndex() const;
    
  private:
    void clearBuffers();
  };
}

#endif /* VulkanFrameAttachment_h */
