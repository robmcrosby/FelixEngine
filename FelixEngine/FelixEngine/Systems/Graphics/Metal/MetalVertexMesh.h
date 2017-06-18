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


namespace fx {
  class Buffer;
  
  class MetalVertexMesh: public VertexMesh {
  public:
    id <MTLDevice> _device;
    id <MTLBuffer> _buffer;
    //std::vector<Buffer*> _buffers;
    
  public:
    MetalVertexMesh(id <MTLDevice> device);
    virtual ~MetalVertexMesh();
    
    virtual bool addVertexBuffer(int size, int count, float *buffer);
  };
}
