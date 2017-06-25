//
//  MetalVertexMesh.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalVertexMesh.h"
#include "VertexMeshData.h"
#include <Metal/Metal.h>

using namespace fx;
using namespace std;

MetalVertexMesh::MetalVertexMesh(id <MTLDevice> device) {
  _device = device;
  _indices = nil;
  _primitive = MTLPrimitiveTypeTriangle;
  _indexCount = 0;
  _vertexCount = 0;
}

MetalVertexMesh::~MetalVertexMesh() {
//  for (Buffer *buffer : _buffers) {
//    delete buffer;
//  }
}

bool MetalVertexMesh::loadData(const VertexMeshData &data) {
  bool success = true;
  
  return success;
}

void MetalVertexMesh::setPrimativeType(VERTEX_PRIMITIVE type) {
  if (type == VERTEX_TRIANGLE_STRIP)
    _primitive = MTLPrimitiveTypeTriangleStrip;
  else
    _primitive = MTLPrimitiveTypeTriangle;
}

bool MetalVertexMesh::setIndexBuffer(int count, int *buffer) {
  NSUInteger length = count * sizeof(int);
  
  _indexCount = (unsigned long)count;
  _indices = [_device newBufferWithBytes:buffer length:length options:MTLResourceOptionCPUCacheModeDefault];
  return _indices != nil;
}

bool MetalVertexMesh::addVertexBuffer(int size, int count, float *buffer) {
  _vertexCount = count;
  
  NSUInteger length = size * count * sizeof(float);
  id <MTLBuffer> mtlBuffer = [_device newBufferWithBytes:buffer length:length options:MTLResourceCPUCacheModeDefaultCache];
  _buffers.push_back(mtlBuffer);
  
  return mtlBuffer != nil;
}

void MetalVertexMesh::encode(id <MTLRenderCommandEncoder> encoder, int instances) {
  NSUInteger index = 0;
  for (id <MTLBuffer> buffer : _buffers) {
    [encoder setVertexBuffer:buffer offset:0 atIndex:index];
    ++index;
  }
  
  MTLPrimitiveType primitive = (MTLPrimitiveType)_primitive;
  
  if (_indices != nil) {
    [encoder drawIndexedPrimitives:primitive
                        indexCount:_indexCount
                         indexType:MTLIndexTypeUInt32
                       indexBuffer:_indices
                 indexBufferOffset:0
                     instanceCount:instances];
  }
  else {
    [encoder drawPrimitives:primitive
                vertexStart:0
                vertexCount:_vertexCount
              instanceCount:instances];
  }
}
