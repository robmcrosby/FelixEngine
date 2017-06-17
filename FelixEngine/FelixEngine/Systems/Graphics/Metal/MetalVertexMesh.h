//
//  MetalVertexMesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"

@protocol MTLDevice;


namespace fx {
  class MTLVertexMeshData;
  
  class MetalVertexMesh: public VertexMesh {
  private:
    MTLVertexMeshData *_data;
    
  public:
    MetalVertexMesh(id <MTLDevice> device);
    virtual ~MetalVertexMesh();
  };
}
