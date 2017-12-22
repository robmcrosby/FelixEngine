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
  _currentBuffer = 0;
  _lastBuffer = 0;
  _bufferCount = 0;
  clearBuffers();
}

MetalUniformBuffer::~MetalUniformBuffer() {
  clearBuffers();
}

bool MetalUniformBuffer::load(void *data, size_t size, BUFFER_COUNT count) {
  bool success = true;
  _bufferSize = size;
  _bufferCount = count;
  _currentBuffer = 0;
  _lastBuffer = 0;
  
  for (int i = 0; i < _bufferCount; ++i) {
    _buffers[i] = [_device newBufferWithBytes:data length:size options:MTLResourceCPUCacheModeDefaultCache];
    success &= _buffers[i] != nil;
  }
  return success;
}

void MetalUniformBuffer::update(void *data, size_t size) {
  size_t dataSize = min(_bufferSize, size);
  _lastBuffer = _currentBuffer;
  _currentBuffer = (_currentBuffer + 1) % _bufferCount;
  memcpy(_buffers[_currentBuffer].contents, data, dataSize);
}

void MetalUniformBuffer::encodeCurrent(id <MTLRenderCommandEncoder> encoder, const string &name, MetalShaderProgram *shader) {
  encode(encoder, name, shader, _currentBuffer);
}

void MetalUniformBuffer::encodeLast(id<MTLRenderCommandEncoder> encoder, const string &name, MetalShaderProgram *shader) {
  encode(encoder, name, shader, _lastBuffer);
}

void MetalUniformBuffer::encode(id <MTLRenderCommandEncoder> encoder, const string &name, MetalShaderProgram *shader, int buffer) {
  if (shader->_vertexIndexMap.count(name)) {
    unsigned long index = shader->_vertexIndexMap[name];
    [encoder setVertexBuffer:_buffers[buffer] offset:0 atIndex:index];
  }
  
  if (shader->_fragmentIndexMap.count(name)) {
    unsigned long index = shader->_fragmentIndexMap[name];
    [encoder setFragmentBuffer:_buffers[buffer] offset:0 atIndex:index];
  }
}

void MetalUniformBuffer::clearBuffers() {
  for (int i = 0; i < MAX_BUFFERS; ++i)
    _buffers[i] = nil;
}
