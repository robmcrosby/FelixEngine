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
#include <sstream>
#include "VertexMeshData.h"
#include "ImageBufferData.h"


namespace fx {
  class XMLTree;
  
  /** define for FileData */
  typedef std::vector<char> FileData;
  
  /** File System */
  class FileSystem {
  protected:
    static FileSystem *instance;
    
    virtual std::string resourcesPath() const = 0;
    virtual std::string documentsPath() const = 0;
    
    virtual bool loadFileData(FileData &data, const std::string &file) const = 0;
    virtual bool loadMeshFile(VertexMeshData &mesh, const std::string &file) const = 0;
    virtual bool loadImageData(ImageBufferData &image, const std::string &file) const = 0;
    virtual bool loadXMLTreeFile(XMLTree &tree, const std::string &file) const = 0;
    
    virtual bool fileExistsAtPath(const std::string &filePath) const = 0;
    virtual std::string findPathForFile(const std::string &file) const = 0;
    
  public:
    virtual ~FileSystem() {}
    
    static std::string getResourcesPath() {
      return instance ? instance->resourcesPath() : "";
    }
    static std::string getDocumentsPath() {
      return instance ? instance->documentsPath() : "";
    }
    
    static bool loadData(FileData &data, const std::string &file) {
      return instance != nullptr && instance->loadFileData(data, file);
    }
    
    static bool loadMesh(VertexMeshData &mesh, const std::string &file) {
      return instance != nullptr && instance->loadMeshFile(mesh, file);
    }
    
    static bool loadImage(ImageBufferData &image, const std::string &file) {
      return instance != nullptr && instance->loadImageData(image, file);
    }
    
    static bool loadXMLTree(XMLTree &tree, const std::string &file) {
      return instance != nullptr && instance->loadXMLTreeFile(tree, file);
    }
    
    static bool fileExists(const std::string &file) {
      return instance != nullptr && instance->fileExistsAtPath(file);
    }
    
    static std::string findFile(const std::string &file) {
      return instance ? instance->findPathForFile(file) : file;
    }
    
    /**
     * @param fileName  String to extract the file postfix from.
     * @return          Either the postfix string or an empty string if none found.
     */
    static std::string getFilePostfix(const std::string &fileName);
    
    /**
     * Extracts the file name from a file path string.
     *
     * @param file  String for the path
     * @return      String for the file name or an empty string if not found
     */
    static std::string getFileName(const std::string &file);
    
    static size_t readB(uint32_t *buffer, size_t count, std::istream &is);
    static size_t readL(uint32_t *buffer, size_t count, std::istream &is);
  };

  static size_t readB(int *buffer, long count, std::istream &is) {return FileSystem::readB((uint32_t*)buffer, count, is);}
  static size_t readB(int &i, std::istream &is) {return FileSystem::readB((uint32_t*)&i, 1, is);}

  static size_t readB(float *buffer, long count, std::istream &is) {return FileSystem::readB((uint32_t*)buffer, count, is);}
  static size_t readB(float &f, std::istream &is) {return FileSystem::readB((uint32_t*)&f, 1, is);}

  static size_t readL(int *buffer, long count, std::istream &is) {return FileSystem::readL((uint32_t*)buffer, count, is);}
  static size_t readL(int &i, std::istream &is) {return FileSystem::readL((uint32_t*)&i, 1, is);}

  static size_t readL(float *buffer, long count, std::istream &is) {return FileSystem::readL((uint32_t*)buffer, count, is);}
  static size_t readL(float &f, std::istream &is) {return FileSystem::readL((uint32_t*)&f, 1, is);}
}

#endif /* FileSystem_h */
