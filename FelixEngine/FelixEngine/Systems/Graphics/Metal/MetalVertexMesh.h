//
//  MetalVertexMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include <vector>

@protocol MTLDevice;
@protocol MTLBuffer;
@protocol MTLRenderCommandEncoder;


namespace fx {
  typedef std::vector<id <MTLBuffer> > bufferList;
  
  class MetalVertexMesh: public VertexMesh {
  public:
    id <MTLDevice> _device;
    id <MTLBuffer> _indices;
    bufferList     _buffers;
    unsigned long  _primitive;
    unsigned long  _indexCount;
    unsigned long  _vertexCount;
    
  public:
    MetalVertexMesh(id <MTLDevice> device);
    virtual ~MetalVertexMesh();
    
    virtual bool loadData(const VertexMeshData &data);
    virtual void setPrimativeType(VERTEX_PRIMITIVE type);
    virtual bool setIndexBuffer(size_t count, const int *buffer);
    virtual bool addVertexBuffer(size_t size, size_t count, const float *buffer);
    
    void encode(id <MTLRenderCommandEncoder> encoder, int instances);
  };
}
