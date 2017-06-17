//
//  MetalFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "Macros.h"

@protocol MTLDevice;


namespace fx {
  class MTLFrameBufferData;
  
  class MetalFrameBuffer: public FrameBuffer {
  private:
    MTLFrameBufferData *_data;
    
  public:
    MetalFrameBuffer(id <MTLDevice> device);
    virtual ~MetalFrameBuffer();
    
    virtual ivec2 size() const;
    virtual float scale() const;
  };
}
