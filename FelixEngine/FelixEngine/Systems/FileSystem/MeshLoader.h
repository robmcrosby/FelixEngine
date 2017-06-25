//
//  MeshLoader.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/24/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef MeshLoader_h
#define MeshLoader_h

#include "FileSystem.h"
#include <sstream>


namespace fx {
  struct MeshLoader {
    static bool loadFromBinaryFile(VertexMeshData &mesh, const std::string &file);
    static bool loadFromBinarySteam(VertexMeshData &mesh, std::istream &is);
    static bool readBufferBinaryStream(VertexMeshData &mesh, int vertexCount, std::istream &is);
    static bool readIndicesBinaryStream(VertexMeshData &mesh, std::istream &is);
  };
}


#endif /* MeshLoader_h */
