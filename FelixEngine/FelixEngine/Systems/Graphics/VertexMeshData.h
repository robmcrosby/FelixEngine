//
//  VertexMeshData.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/25/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef VertexMeshData_h
#define VertexMeshData_h

#include "GraphicResources.h"
#include "IndexedMap.h"


namespace fx {
  typedef std::vector<int> IndexBuffer;
  typedef std::vector<float> VertexBuffer;
  typedef std::vector<ivec2> Ranges;
  
  struct VertexMeshData {
    VERTEX_PRIMITIVE primitiveType;
    int totalVertices;
    
    IndexBuffer indices;
    Ranges      subMeshes;
    IndexedMap<std::string, VertexBuffer> buffers;
    
    VertexMeshData(): primitiveType(VERTEX_TRIANGLES), totalVertices(0) {}
  };
}


#endif /* VertexMeshData_h */
