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
    static bool loadFromBinaryFile(MeshData &mesh, const std::string &file);
    static bool loadFromBinarySteam(MeshData &mesh, std::istream &is);
  };
}


#endif /* MeshLoader_h */
