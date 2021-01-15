//
//  MetalFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "MetalTextureBuffer.h"
#include <vector>


#ifndef MetalFrameBuffer_h
#define MetalFrameBuffer_h

@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLCommandQueue;
@protocol MTLCommandBuffer;
@protocol MTLRenderCommandEncoder;
@protocol CAMetalDrawable;
@protocol MTLCommandBuffer;


@class CAMetalLayer;
@class MTLRenderPipelineDescriptor;


namespace fx {
  typedef std::vector<id <MTLTexture> > attachmentsList;
  class BlendState;
  
  class MetalFrameBuffer: public FrameBuffer {
  public:
    id <MTLDevice> _device;
    id <MTLCommandQueue> _queue;
    id <MTLTexture> _depthAttachment;
    id <MTLTexture> _stencilAttachment;
    attachmentsList _colorAttachments;
    
    CAMetalLayer *_metalLayer;
    id <CAMetalDrawable> _drawable;
    
    ivec2 _size;
    int _idFlag;
    
  public:
    MetalFrameBuffer(id <MTLDevice> device, id <MTLCommandQueue> queue);
    virtual ~MetalFrameBuffer();
    
    virtual bool setToWindow(int index);
    
    virtual bool resize(int width, int height);
    virtual ivec2 size() const;
    
    virtual bool addRenderBuffer(TEXTURE_FORMAT format, TEXTURE_ACCESS access);
    
    virtual TextureBufferPtr getColorTexture(int index);
    
    void setMetalLayer(CAMetalLayer *layer);
    void present(id <MTLCommandBuffer> buffer);
    
    int pipelineId(const BlendState &blending) const;
    
    id <MTLRenderCommandEncoder> createEncoder(id<MTLCommandBuffer> buffer, const ActionState &depthStencilAction, const ActionState *colorActions);
    MTLRenderPipelineDescriptor* createPipelineDescriptor(const BlendState &blending) const;
    
  private:
    bool addBuffer(TEXTURE_FORMAT format, bool readable);
    
    id <MTLTexture> resizeTexture(id <MTLTexture> texture, ivec2 size);
    void getNextDrawable();
    int metalBlendFactor(int factor) const;
    int metalBlendOperation(int operation) const;
  };
}

#endif /* MetalFrameBuffer_h */
