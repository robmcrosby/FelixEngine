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

MetalVertexMesh::MetalVertexMesh(id <MTLDevice> device): _device(device) {
  
}

MetalVertexMesh::~MetalVertexMesh() {
//  for (Buffer *buffer : _buffers) {
//    delete buffer;
//  }
}

bool MetalVertexMesh::loadData(const VertexMeshData &data) {
  return false;
}

void MetalVertexMesh::setPrimativeType(VERTEX_PRIMITIVE type) {
  
}

bool MetalVertexMesh::setIndexBuffer(int count, int *buffer) {
  return false;
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
  
  [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:_vertexCount instanceCount:instances];
}
