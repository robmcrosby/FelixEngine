//
//  MetalVertexMesh.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalVertexMesh.h"
#include "MetalShaderProgram.h"
#include "VertexMeshData.h"
#include "GraphicTask.h"
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
  
}

bool MetalVertexMesh::loadData(const VertexMeshData &data) {
  bool success = true;
  
  setPrimativeType(data.primitiveType);
  if (data.indices.size() > 0)
    success = setIndexBuffer(data.indices.size(), &data.indices[0]);
  
  if (success) {
    for (auto buffer : data.buffers) {
      size_t size = buffer.second.size()/data.totalVertices;
      success &= addVertexBuffer(buffer.first, size, data.totalVertices, &buffer.second[0]);
    }
  }
  
  return success;
}

void MetalVertexMesh::setPrimativeType(VERTEX_PRIMITIVE type) {
  if (type == VERTEX_TRIANGLE_STRIP)
    _primitive = MTLPrimitiveTypeTriangleStrip;
  else
    _primitive = MTLPrimitiveTypeTriangle;
}

bool MetalVertexMesh::setIndexBuffer(size_t count, const int *buffer) {
  NSUInteger length = count * sizeof(int);
  
  _indexCount = (unsigned long)count;
  _indices = [_device newBufferWithBytes:buffer length:length options:MTLResourceOptionCPUCacheModeDefault];
  return _indices != nil;
}

bool MetalVertexMesh::addVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer) {
  _vertexCount = count;
  
  NSUInteger length = size * count * sizeof(float);
  id <MTLBuffer> mtlBuffer = [_device newBufferWithBytes:buffer length:length options:MTLResourceCPUCacheModeDefaultCache];
  _buffers[name] = mtlBuffer;
  
  return mtlBuffer != nil;
}

void MetalVertexMesh::encode(id <MTLRenderCommandEncoder> encoder, MetalShaderProgram *shader, int instances) {
  for (auto buffer : _buffers) {
    if (shader->_vertexIndexMap.count(buffer.first)) {
      NSUInteger index = shader->_vertexIndexMap.at(buffer.first);
      [encoder setVertexBuffer:buffer.second offset:0 atIndex:index];
    }
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

void MetalVertexMesh::encode(id <MTLRenderCommandEncoder> encoder, const GraphicTask &task) {
  MetalShaderProgram *shader = static_cast<MetalShaderProgram*>(task.shader);
  
  // Set the Triangle Culling Mode
  if (task.cullMode == CULL_FRONT)
    [encoder setCullMode:MTLCullModeFront];
  else if (task.cullMode == CULL_BACK)
    [encoder setCullMode:MTLCullModeBack];
  
  // Set the Vertex Buffers
  for (auto buffer : _buffers) {
    if (shader->_vertexIndexMap.count(buffer.first)) {
      NSUInteger index = shader->_vertexIndexMap.at(buffer.first);
      [encoder setVertexBuffer:buffer.second offset:0 atIndex:index];
    }
  }
  
  // Draw the Mesh
  MTLPrimitiveType primitive = (MTLPrimitiveType)_primitive;
  if (_indices != nil) {
    [encoder drawIndexedPrimitives:primitive
                        indexCount:_indexCount
                         indexType:MTLIndexTypeUInt32
                       indexBuffer:_indices
                 indexBufferOffset:0
                     instanceCount:task.instances];
  }
  else {
    [encoder drawPrimitives:primitive
                vertexStart:0
                vertexCount:_vertexCount
              instanceCount:task.instances];
  }
}
