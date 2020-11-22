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
  class USDCrate;
  class USDItem;

  struct MeshLoader {
    static bool loadFromBinaryFile(VertexMeshData &mesh, const std::string &file);
    static bool loadFromBinarySteam(VertexMeshData &mesh, std::istream &is);
    static bool readBufferBinaryStream(VertexMeshData &mesh, int vertexCount, std::istream &is);
    static bool readIndicesBinaryStream(VertexMeshData &mesh, std::istream &is);
    
    static bool loadFromCrateFile(VertexMeshData &mesh, const USDCrate &crate);
    static bool loadFromCrateFile(VertexMeshData &mesh, const USDItem &item);
  };
}


#endif /* MeshLoader_h */
