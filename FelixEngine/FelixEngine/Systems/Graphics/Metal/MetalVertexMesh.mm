//
//  MetalVertexMesh.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalVertexMesh.h"
#include <Metal/Metal.h>

//namespace fx {
//  struct Buffer {
//    id <MTLBuffer> buffer;
//    int size, count;
//    
//    Buffer(): buffer(nil), size(1), count(0) {}
//    ~Buffer() {}
//  };
//}


using namespace fx;
using namespace std;

MetalVertexMesh::MetalVertexMesh(id <MTLDevice> device): _device(device) {
  
}

MetalVertexMesh::~MetalVertexMesh() {
//  for (Buffer *buffer : _buffers) {
//    delete buffer;
//  }
}

bool MetalVertexMesh::addVertexBuffer(int size, int count, float *buffer) {
  //Buffer *buff = new Buffer();
  
  //buff->size = size;
  //buff->count = count;
  
  NSUInteger length = size * count * sizeof(float);
  //buff->buffer = [_device newBufferWithBytes:buffer length:length options:MTLResourceCPUCacheModeDefaultCache];
  _buffer = [_device newBufferWithBytes:buffer length:length options:MTLResourceCPUCacheModeDefaultCache];
  
  //_buffers.push_back(buff);
  return _buffer != nil; //buff->buffer != nil;
}
