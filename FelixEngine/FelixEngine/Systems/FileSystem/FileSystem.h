//
//  FileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef FileSystem_h
#define FileSystem_h

#include <string>
#include "Variant.h"
#include "IndexedMap.h"

namespace fx {
  
  typedef IndexedMap<std::string, Variant> MeshData;
  
  /** File System */
  class FileSystem {
  protected:
    static FileSystem *instance;
    
    virtual std::string resourcesPath() const = 0;
    virtual std::string documentsPath() const = 0;
    
    virtual bool loadMeshFile(MeshData &mesh, const std::string &file) const = 0;
    
  public:
    virtual ~FileSystem() {}
    
    static std::string getResourcesPath() {
      return instance ? instance->resourcesPath() : "";
    }
    static std::string getDocumentsPath() {
      return instance ? instance->documentsPath() : "";
    }
    
    static bool loadMesh(MeshData &mesh, const std::string &file) {
      return instance ? instance->loadMeshFile(mesh, file) : false;
    }
  };
  
}

#endif /* FileSystem_h */
