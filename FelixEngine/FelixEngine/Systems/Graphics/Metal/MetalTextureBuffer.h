//
//  MetalTextureBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"

@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLRenderCommandEncoder;
@protocol MTLSamplerState;


namespace fx {
  class MetalTextureBuffer: public TextureBuffer {
  public:
    id <MTLDevice> _device;
    id <MTLTexture> _texture;
    int _width, _height;
 
  public:
    MetalTextureBuffer(id <MTLDevice> device);
    MetalTextureBuffer(id <MTLDevice> device, id <MTLTexture> texture);
    virtual ~MetalTextureBuffer();
    
    virtual bool load(const ImageBufferData &data);
    virtual ivec2 size() const;
    
    void encode(id <MTLRenderCommandEncoder> encoder, id <MTLSamplerState> sampler, int index);
    void setMetalTexture(id <MTLTexture> texture);
  };
}
