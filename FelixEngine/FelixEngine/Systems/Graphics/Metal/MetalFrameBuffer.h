//
//  MetalFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include <vector>

@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLCommandBuffer;
@protocol MTLRenderCommandEncoder;
@protocol CAMetalDrawable;
@protocol MTLCommandBuffer;


@class CAMetalLayer;


namespace fx {
  typedef std::vector<id <MTLTexture> > attachmentsList;
  class GraphicTask;
  
  class MetalFrameBuffer: public FrameBuffer {
  public:
    id <MTLDevice>  _device;
    id <MTLTexture> _depthAttachment;
    id <MTLTexture> _stencilAttachment;
    attachmentsList _colorAttachments;
    
    CAMetalLayer *_metalLayer;
    id <CAMetalDrawable> _drawable;
    
    ivec2 _size;
    
  public:
    MetalFrameBuffer(id <MTLDevice> device);
    virtual ~MetalFrameBuffer();
    
    virtual bool resize(int width, int height);
    virtual ivec2 size() const;
    
    virtual bool addDepthBuffer();
    virtual bool addColorTexture();
    
    virtual TextureBuffer* getColorTexture(int index);
    
    void setMetalLayer(CAMetalLayer *layer);
    void present(id <MTLCommandBuffer> buffer);
    
    id <MTLRenderCommandEncoder> createEncoder(id<MTLCommandBuffer> buffer, const GraphicTask &task);
    
  private:
    id <MTLTexture> resizeTexture(id <MTLTexture> texture, ivec2 size);
    void getNextDrawable();
  };
}
