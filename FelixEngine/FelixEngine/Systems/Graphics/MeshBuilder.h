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
    static VertexMeshPtr createPlane(vec2 size = vec2(2.0f, 2.0f));
    static VertexMeshPtr createCube(float size = 1.0f);
    
    static bool loadPrimitiveToMesh(VertexMesh *mesh, const XMLTree::Node &node);
    static void loadPlaneToMesh(VertexMesh *mesh, vec2 size = vec2(2.0f, 2.0f));
  };
}

#endif /* MeshBuilder_h */
