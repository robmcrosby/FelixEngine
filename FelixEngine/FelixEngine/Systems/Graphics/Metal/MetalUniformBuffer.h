//
//  MetalUniformBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/26/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include <map>

@protocol MTLDevice;
@protocol MTLBuffer;
@protocol MTLRenderCommandEncoder;


namespace fx {
  
  class MetalUniformBuffer: public UniformBuffer {
  public:
    id <MTLDevice> _device;
    
  public:
    MetalUniformBuffer(id <MTLDevice> device);
    virtual ~MetalUniformBuffer();
    
    virtual bool setNumberOfBuffers(int count);
    virtual bool update(const void *data, size_t size);
  };
}
