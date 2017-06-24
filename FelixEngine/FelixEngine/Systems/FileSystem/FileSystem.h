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
    
    virtual bool fileExistsAtPath(const std::string &filePath) const = 0;
    
  public:
    virtual ~FileSystem() {}
    
    static std::string getResourcesPath() {
      return instance ? instance->resourcesPath() : "";
    }
    static std::string getDocumentsPath() {
      return instance ? instance->documentsPath() : "";
    }
    
    static bool loadMesh(MeshData &mesh, const std::string &file) {
      return instance != nullptr && instance->loadMeshFile(mesh, file);
    }
    
    static bool fileExists(const std::string &file) {
      return instance != nullptr && instance->fileExistsAtPath(file);
    }
    
    /**
     * @param fileName  String to extract the file postfix from.
     * @return          Either the postfix string or an empty string if none found.
     */
    static std::string getFilePostfix(const std::string &fileName)
    {
      size_t loc = fileName.rfind(".");
      if (loc != std::string::npos)
        return fileName.substr(loc+1);
      return "";
    }
    
    /**
     * Extracts the file name from a file path string.
     *
     * @param path  String for the path.
     * @param name  String for the file name.
     */
    static bool getFileNameFromPath(const std::string &path, std::string &name)
    {
      bool success = false;
      std::string::size_type start, end, npos = std::string::npos;
      
      start = path.find_last_of('/');
      if (start == npos)
        start = 0;
      else
        start++;
      
      end = path.find_last_of('.');
      
      if (end != npos && start < end)
      {
        name = path.substr(start, end-start);
        success = true;
      }
      
      return success;
    }
  };
  
}

#endif /* FileSystem_h */
