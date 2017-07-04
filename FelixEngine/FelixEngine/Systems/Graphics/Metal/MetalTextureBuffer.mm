//
//  MetalTextureBuffer.m
//  FelixEngine
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalTextureBuffer.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalTextureBuffer::MetalTextureBuffer(id <MTLDevice> device): _device(device) {
  
}

MetalTextureBuffer::~MetalTextureBuffer() {
  
}

bool MetalTextureBuffer::load(const ImageBufferData &data) {
  return false;
}

ivec2 MetalTextureBuffer::size() const {
  return ivec2();
}

void MetalTextureBuffer::encode(id <MTLRenderCommandEncoder> encoder, id <MTLSamplerState> sampler, int index) {
  
}
