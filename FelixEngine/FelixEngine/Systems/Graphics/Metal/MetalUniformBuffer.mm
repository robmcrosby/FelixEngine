//
//  MetalUniformBuffer.m
//  FelixEngine
//
//  Created by Robert Crosby on 6/26/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalUniformBuffer.h"
#include "MetalShaderProgram.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;


MetalUniformBuffer::MetalUniformBuffer(id <MTLDevice> device): _device(device) {
  _buffer = nil;
}

MetalUniformBuffer::~MetalUniformBuffer() {
  
}

bool MetalUniformBuffer::load(const void *data, size_t size) {
  _buffer = [_device newBufferWithBytes:data length:size options:MTLResourceCPUCacheModeDefaultCache];
  return _buffer != nil;
}

bool MetalUniformBuffer::update(const void *data, size_t size) {
  return false;
}

void MetalUniformBuffer::encode(id <MTLRenderCommandEncoder> encoder, const std::string &name, MetalShaderProgram *shader) {
  if (shader->_vertexIndexMap.count(name)) {
    [encoder setVertexBuffer:_buffer offset:0 atIndex:shader->_vertexIndexMap[name]];
  }
  if (shader->_fragmentIndexMap.count(name)) {
    [encoder setFragmentBuffer:_buffer offset:0 atIndex:shader->_vertexIndexMap[name]];
  }
}
