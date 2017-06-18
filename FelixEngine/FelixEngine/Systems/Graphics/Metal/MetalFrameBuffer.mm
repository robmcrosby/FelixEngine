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

id <MTLRenderCommandEncoder> MetalFrameBuffer::createEncoder(id<MTLCommandBuffer> buffer) {
  MTLRenderPassDescriptor *descriptor = [[MTLRenderPassDescriptor alloc] init];
  
  int index = 0;
  MTLClearColor clearColor = MTLClearColorMake(0.0, 104.0/255.0, 5.0/255.0, 1.0);
  for (id <MTLTexture> texture : _textures) {
    descriptor.colorAttachments[index].texture = texture;
    descriptor.colorAttachments[index].loadAction = MTLLoadActionClear;
    descriptor.colorAttachments[index].clearColor = clearColor;
    ++index;
  }
  
  return [buffer renderCommandEncoderWithDescriptor:descriptor];
}
