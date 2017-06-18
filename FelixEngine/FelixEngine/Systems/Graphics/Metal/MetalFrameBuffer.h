//
//  MetalFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "Macros.h"
#include <vector>

@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLCommandBuffer;
@protocol MTLRenderCommandEncoder;


namespace fx {
  typedef std::vector<id <MTLTexture> > textureList;
  
  class MetalFrameBuffer: public FrameBuffer {
  public:
    id <MTLDevice> _device;
    textureList    _textures;
    
  public:
    MetalFrameBuffer(id <MTLDevice> device);
    virtual ~MetalFrameBuffer();
    
    virtual ivec2 size() const;
    virtual float scale() const;
    
    id <MTLRenderCommandEncoder> createEncoder(id<MTLCommandBuffer> buffer);
  };
}
