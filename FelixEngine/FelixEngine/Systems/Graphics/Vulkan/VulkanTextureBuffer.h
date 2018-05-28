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
    
    static std::map<int, VkSampler> samplerMap;
    
  public:
    VulkanTextureBuffer();
    virtual ~VulkanTextureBuffer();
    
    bool load(const ImageBufferData &data);
    bool loaded() const;
    ivec2 size() const;
    
    virtual void setDefaultSampler(SamplerState state);
    virtual SamplerState defaultSampler() const;
    
    VkImageView getImageView() const {return _textureImageView;}
    
    static VkSampler getSamplerForState(const SamplerState &state);
    static VkSamplerAddressMode getAddressMode(COORD_TYPE type);
    
  private:
    void clearTexture();
  };
}

#endif /* VulkanTextureBuffer_h */
