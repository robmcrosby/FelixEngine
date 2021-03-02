//
//  MetalContext.h
//  FelixEngine
//
//  Created by Robert Crosby on 3/2/21.
//  Copyright © 2021 Robert Crosby. All rights reserved.
//

#ifndef MetalContext_h
#define MetalContext_h

#include <Metal/Metal.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/CAMetalLayer.h>

#include "GraphicResources.h"
#include "MetalRenderPass.h"
#include "MetalFrameBuffer.h"


namespace fx {
  struct MetalContext {
    id <MTLDevice>        device;
    id <MTLCommandQueue>  queue;
    
    UIView *view;
    CAMetalLayer *metalLayer;
    
    dispatch_semaphore_t frameBoundarySemaphore;
    
    std::vector<MetalRenderPassPtr> renderPasses;
    MetalFrameBufferPtr windowBuffer;
    
    ~MetalContext() {}
  };
}

#endif /* MetalContext_h */
