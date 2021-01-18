//
//  MetalRenderPass.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef MetalRenderPass_h
#define MetalRenderPass_h

#include "RenderPass.h"
#include "Macros.h"


@protocol MTLDevice;
@protocol MTLCommandBuffer;

OBJC_CLASS(MetalDepthStencil)
OBJC_CLASS(MetalTextureSampler)

namespace fx {
  
  /**
   *
   */
  class MetalRenderPass: public RenderPass {
    public:
    id <MTLDevice>        _device;
    id <MTLCommandBuffer> _buffer;
    
    MetalDepthStencil   *_depthStencilStates;
    MetalTextureSampler *_samplerStates;
    
    public:
    MetalRenderPass(id <MTLDevice> device);
    virtual ~MetalRenderPass();
    
    virtual void render();
    void encodeRender(id <MTLCommandBuffer> buffer);
    void setCommandBuffer(id <MTLCommandBuffer> buffer);
  };

  typedef std::shared_ptr<MetalRenderPass> MetalRenderPassPtr;

}

#endif /* MetalRenderPass_h */
