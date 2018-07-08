//
//  MetalRenderItem.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/7/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef MetalRenderItem_h
#define MetalRenderItem_h

#include "RenderItem.h"

@protocol MTLDevice;


namespace fx {
  
  /**
   *
   */
  class MetalRenderItem: public RenderItem {
    public:
    id <MTLDevice> _device;
    
    public:
    MetalRenderItem(id <MTLDevice> device);
    virtual ~MetalRenderItem();
  };
}

#endif /* MetalRenderItem_h */
