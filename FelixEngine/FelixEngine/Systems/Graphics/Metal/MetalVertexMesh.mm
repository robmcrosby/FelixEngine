//
//  MetalVertexMesh.mm
//  FelixEngine
//
//  Created by Robert Crosby on 6/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MetalVertexMesh.h"
#include <Metal/Metal.h>


namespace fx {
  struct MTLVertexMeshData {
    id <MTLDevice> device;
    
    ~MTLVertexMeshData() {}
  };
}


using namespace fx;
using namespace std;

MetalVertexMesh::MetalVertexMesh(id <MTLDevice> device): _data(new MTLVertexMeshData()) {
  _data->device = device;
}

MetalVertexMesh::~MetalVertexMesh() {
  delete _data;
}
