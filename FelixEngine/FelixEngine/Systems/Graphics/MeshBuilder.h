//
//  MeshBuilder.h
//  FelixEngine
//
//  Created by Robert Crosby on 3/14/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef MeshBuilder_h
#define MeshBuilder_h

#include "GraphicResources.h"

namespace fx {
  struct MeshBuilder {
    static VertexMeshPtr createFromFile(const std::string &file);
    static VertexMeshPtr createCube(float size = 1.0f);
  };
}

#endif /* MeshBuilder_h */
