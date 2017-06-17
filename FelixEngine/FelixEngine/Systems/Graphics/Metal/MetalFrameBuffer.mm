//
//  MetalFrameBuffer.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalFrameBuffer.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalFrameBuffer::MetalFrameBuffer(id <MTLDevice> device): _device(device) {
  
}

MetalFrameBuffer::~MetalFrameBuffer() {
  
}

ivec2 MetalFrameBuffer::size() const {
  return ivec2();
}

float MetalFrameBuffer::scale() const {
  return 1.0f;
}
