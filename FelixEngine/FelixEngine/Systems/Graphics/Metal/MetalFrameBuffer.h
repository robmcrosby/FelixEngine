//
//  MetalFrameBuffer.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"


namespace fx {
  class MetalFrameBuffer: public FrameBuffer {
  public:
    MetalFrameBuffer();
    virtual ~MetalFrameBuffer();
  };
}
