//
//  MetalCommandBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/18/21.
//  Copyright © 2021 Robert Crosby. All rights reserved.
//

#ifndef MetalCommandBuffer_h
#define MetalCommandBuffer_h

#include "GraphicResources.h"
#include "Macros.h"
//#include "RenderPass.h"


@protocol MTLCommandBuffer;

//OBJC_CLASS(MetalDepthStencil)
//OBJC_CLASS(MetalTextureSampler)

namespace fx {
  
  /**
   *
   */
  class MetalCommandBuffer: public CommandBuffer {
  public:
    id <MTLCommandBuffer> _buffer;
    
  public:
    MetalCommandBuffer(id <MTLCommandBuffer> buffer);
    virtual ~MetalCommandBuffer();
    
    virtual void encodeRenderPass(RenderPassPtr pass);
    virtual void encodeBlitTexture(TextureBufferPtr src, TextureBufferPtr dst, int slice, int level);
    virtual void encodeGenerateMipmap(TextureBufferPtr texture);
    virtual void commit();
  };
}

#endif /* MetalCommandBuffer_h */
