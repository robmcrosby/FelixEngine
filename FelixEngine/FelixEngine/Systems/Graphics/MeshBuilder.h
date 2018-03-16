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
    static VertexMeshPtr loadFromFile(const std::string &file);
  };
}

#endif /* MeshBuilder_h */
