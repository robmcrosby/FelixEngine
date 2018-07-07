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
    VkFormat       _textureFormat;
    VkImage        _textureImage;
    VkDeviceMemory _textureImageMemory;
    VkImageView    _textureImageView;
    
    int _width, _height;
    
    static std::map<int, VkSampler> samplerMap;
    
  public:
    VulkanTextureBuffer();
    virtual ~VulkanTextureBuffer();
    
    bool load(const ImageBufferData &data);
    bool loadSwapBuffer(VkFormat format, VkImage image, int width, int height);
    bool loadDepthBuffer(VkFormat format, int width, int height);
    bool loaded() const;
    ivec2 size() const;
    
    virtual void setDefaultSampler(SamplerState state);
    virtual SamplerState defaultSampler() const;
    
    VkFormat getFormat() const {return _textureFormat;}
    VkImageView getImageView() const {return _textureImageView;}
    
    static VkSampler getSamplerForState(const SamplerState &state);
    static VkSamplerAddressMode getAddressMode(COORD_TYPE type);
    
  private:
    void clearTexture();
  };
}

#endif /* VulkanTextureBuffer_h */
