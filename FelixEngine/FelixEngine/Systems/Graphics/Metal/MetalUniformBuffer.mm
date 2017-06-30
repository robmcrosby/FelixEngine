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
  _current = 0;
  clearBuffers();
}

MetalUniformBuffer::~MetalUniformBuffer() {
  clearBuffers();
}

bool MetalUniformBuffer::load(void *data, size_t size) {
  bool success = true;
  _size = size;
  _data = data;
  
  for (int i = 0; i < MAX_BUFFERS; ++i) {
    _buffers[i] = [_device newBufferWithBytes:data length:size options:MTLResourceCPUCacheModeDefaultCache];
    success &= _buffers[i] != nil;
  }
  return success;
}

void MetalUniformBuffer::update() {
  if (_buffers[_current] != nil) {
    _current = _current + 1 % MAX_BUFFERS;
    memcpy(_buffers[_current].contents, _data, _size);
  }
}

void MetalUniformBuffer::encode(id <MTLRenderCommandEncoder> encoder, const std::string &name, MetalShaderProgram *shader) {
  if (shader->_vertexIndexMap.count(name)) {
    [encoder setVertexBuffer:_buffers[_current] offset:0 atIndex:shader->_vertexIndexMap[name]];
  }
  if (shader->_fragmentIndexMap.count(name)) {
    [encoder setFragmentBuffer:_buffers[_current] offset:0 atIndex:shader->_vertexIndexMap[name]];
  }
}

void MetalUniformBuffer::clearBuffers() {
  for (int i = 0; i < MAX_BUFFERS; ++i)
    _buffers[i] = nil;
}
