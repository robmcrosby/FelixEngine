//
//  iOSFileSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "FileSystem.h"

namespace fx {
  
  /** iOS File System */
  class iOSFileSystem: public FileSystem {
  public:
    iOSFileSystem();
    virtual ~iOSFileSystem();
    
  protected:
    virtual std::string resourcesPath() const;
    virtual std::string documentsPath() const;
    
    virtual bool loadMeshFile(MeshData &mesh, const std::string &file) const;
    
    virtual bool fileExistsAtPath(const std::string &filePath) const;
  };
  
}
