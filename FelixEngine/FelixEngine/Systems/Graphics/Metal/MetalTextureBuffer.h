//
//  MetalTextureBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MetalTextureBuffer_h
#define MetalTextureBuffer_h

#include "GraphicResources.h"


@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLCommandQueue;
@protocol MTLRenderCommandEncoder;
@protocol MTLSamplerState;


namespace fx {

  class MetalTextureBuffer: public TextureBuffer {
  public:
    id <MTLDevice> _device;
    id <MTLTexture> _texture;
    id <MTLCommandQueue> _queue;
    int _width, _height, _mipLevels, _usageFlags;
    bool _loaded;
    SamplerState _samplerState;
    vec4 _default;
 
  public:
    MetalTextureBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue);
    MetalTextureBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue, id <MTLTexture> texture);
    virtual ~MetalTextureBuffer();
    
    virtual bool loadBuffer(ivec2 size, TEXTURE_FORMAT format, int usageFlags);
    virtual bool loadImage(const ImageBufferData &image, bool generateMipMap);
    virtual bool loadCubeMap(const ImageBufferSet &images, bool generateMipMap);
    virtual bool loadCubeMap(const ImageBufferData &image, bool generateMipMap);
    
    virtual bool loaded() const;
    virtual ivec2 size() const;
    
    virtual int usageFlags() const;
    virtual TEXTURE_FORMAT format() const;
    
    virtual void setDefaultSampler(SamplerState state) {_samplerState = state;}
    virtual SamplerState defaultSampler() const {return _samplerState;}
    
    void encode(id <MTLRenderCommandEncoder> encoder, id <MTLSamplerState> sampler, unsigned long index);
    void setMetalTexture(id <MTLTexture> texture);
    
    id <MTLTexture> getMetalTexture() const {return _texture;}
    
  private:
    void encodeGenerateMipMap();
    void encodeGenerateCubeMap(const ImageBufferData &image, bool generateMipMap);
  };
}

#endif /* MetalTextureBuffer_h */
