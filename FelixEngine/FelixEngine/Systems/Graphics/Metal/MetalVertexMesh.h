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
    bufferList     _buffers;
    int _vertexCount;
    
  public:
    MetalVertexMesh(id <MTLDevice> device);
    virtual ~MetalVertexMesh();
    
    virtual bool loadData(const VertexMeshData &data);
    virtual void setPrimativeType(VERTEX_PRIMITIVE type);
    virtual bool setIndexBuffer(int count, int *buffer);
    virtual bool addVertexBuffer(int size, int count, float *buffer);
    
    void encode(id <MTLRenderCommandEncoder> encoder, int instances);
  };
}
