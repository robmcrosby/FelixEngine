//
//  VulkanTextureBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef VulkanTextureBuffer_h
#define VulkanTextureBuffer_h

#include "GraphicResources.h"
#include <vulkan/vulkan.h>


namespace fx {
  
  /**
   *
   */
  class VulkanTextureBuffer: public TextureBuffer {
  private:
    VkImage        _textureImage;
    VkDeviceMemory _textureImageMemory;
    VkImageView    _textureImageView;
    
  public:
    VulkanTextureBuffer();
    virtual ~VulkanTextureBuffer();
    
    bool load(const ImageBufferData &data);
    bool loaded() const;
    ivec2 size() const;
    
    virtual void setDefaultSampler(SamplerState state);
    virtual SamplerState defaultSampler() const;
    
  private:
    void clearTexture();
  };
}

#endif /* VulkanTextureBuffer_h */
