//
//  MetalVertexMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MetalVertexMesh_h
#define MetalVertexMesh_h

#include "GraphicResources.h"
#include <map>


@protocol MTLDevice;
@protocol MTLBuffer;
@protocol MTLRenderCommandEncoder;


namespace fx {
  class MetalShaderProgram;
  typedef std::map<std::string, id <MTLBuffer> > MetalBufferMap;
  
  class MetalVertexMesh: public VertexMesh {
  public:
    id <MTLDevice> _device;
    id <MTLBuffer> _indices;
    MetalBufferMap _buffers;
    unsigned long  _primitive;
    unsigned long  _indexCount;
    unsigned long  _vertexCount;
    
  private:
    static unsigned int meshCount;
    unsigned int _meshId;
    
  public:
    MetalVertexMesh(id <MTLDevice> device);
    virtual ~MetalVertexMesh();
    
    virtual bool load(const VertexMeshData &data);
    virtual void setPrimativeType(VERTEX_PRIMITIVE type);
    virtual bool setIndexBuffer(size_t count, const int *buffer);
    virtual bool setVertexBuffer(const std::string &name, size_t size, size_t count, const float *buffer);
    virtual bool loadBuffers();
    
    virtual unsigned int getMeshId() const;
    
    void encode(id <MTLRenderCommandEncoder> encoder, MetalShaderProgram *shader, int instances);
  };
}

#endif /* MetalVertexMesh_h */
