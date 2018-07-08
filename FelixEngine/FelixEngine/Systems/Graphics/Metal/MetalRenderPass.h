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

@protocol MTLDevice;

namespace fx {
  
  /**
   *
   */
  class MetalRenderPass: public RenderPass {
    public:
    id <MTLDevice> _device;
    
    public:
    MetalRenderPass(id <MTLDevice> device);
    virtual ~MetalRenderPass();
  };
}

#endif /* MetalRenderPass_h */
