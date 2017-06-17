//
//  MetalFrameBuffer.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalFrameBuffer.h"
#include <Metal/Metal.h>


namespace fx {
  struct MTLFrameBufferData {
    id <MTLDevice> device;
    
    ~MTLFrameBufferData() {}
  };
}


using namespace fx;
using namespace std;


MetalFrameBuffer::MetalFrameBuffer(id <MTLDevice> device): _data(new MTLFrameBufferData()) {
  _data->device = device;
}

MetalFrameBuffer::~MetalFrameBuffer() {
  delete _data;
}

ivec2 MetalFrameBuffer::size() const {
  return ivec2();
}

float MetalFrameBuffer::scale() const {
  return 1.0f;
}
