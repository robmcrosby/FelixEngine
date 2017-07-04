//
//  MetalTextureBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"

@protocol MTLDevice;
@protocol MTLBuffer;
@protocol MTLRenderCommandEncoder;
@protocol MTLSamplerState;


namespace fx {
  class MetalTextureBuffer: public TextureBuffer {
  public:
    id <MTLDevice> _device;
 
  public:
    MetalTextureBuffer(id <MTLDevice> device);
    virtual ~MetalTextureBuffer();
    
    virtual bool load(const ImageBufferData &data);
    virtual ivec2 size() const;
    
    void encode(id <MTLRenderCommandEncoder> encoder, id <MTLSamplerState> sampler, int index);
  };
}