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
#include <vector>
#include <map>


namespace fx {
  typedef std::vector<int> IndexBuffer;
  typedef std::vector<float> VertexBuffer;
  typedef std::map<std::string, VertexBuffer> BufferMap;
  typedef std::vector<ivec2> Ranges;
  
  struct VertexMeshData {
    VERTEX_PRIMITIVE primitiveType;
    int totalVertices;
    
    IndexBuffer indices;
    Ranges      subMeshes;
    BufferMap   buffers;
    
    VertexMeshData(): primitiveType(VERTEX_TRIANGLES), totalVertices(0) {}
  };
}


#endif /* VertexMeshData_h */
