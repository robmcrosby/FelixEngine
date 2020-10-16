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
#include <FelixEngine/VertexMeshData.h>
#include <FelixEngine/ImageBufferData.h>


namespace fx {
  class XMLTree;
  
  /** define for FileData */
  typedef std::vector<char> FileData;

  /** define for IntVector */
  typedef std::vector<int> IntVector;



  class membuf : public std::basic_streambuf<char> {
  public:
    membuf(const uint8_t *p, size_t l) {
      setg((char*)p, (char*)p, (char*)p + l);
    }
  };

  class memstream : public std::istream {
  public:
    memstream(const uint8_t *p, size_t l) :
      std::istream(&_buffer),
      _buffer(p, l) {
        rdbuf(&_buffer);
      }
  private:
    membuf _buffer;
  };
  

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
    
    static size_t readB(uint16_t *buffer, size_t count, std::istream &is);
    static size_t readL(uint16_t *buffer, size_t count, std::istream &is);
    
    static size_t readB(uint32_t *buffer, size_t count, std::istream &is);
    static size_t readL(uint32_t *buffer, size_t count, std::istream &is);
    
    static size_t readB(uint64_t *buffer, size_t count, std::istream &is);
    static size_t readL(uint64_t *buffer, size_t count, std::istream &is);
  };

  static inline size_t readB(short *buffer, long count, std::istream &is) {
    return FileSystem::readB((uint16_t*)buffer, count, is);
  }

  static inline size_t readB(short &i, std::istream &is) {
    return FileSystem::readB((uint16_t*)&i, 1, is);
  }

  static inline size_t readB(int *buffer, long count, std::istream &is) {
    return FileSystem::readB((uint32_t*)buffer, count, is);
  }

  static inline size_t readB(int &i, std::istream &is) {
    return FileSystem::readB((uint32_t*)&i, 1, is);
  }

  static inline size_t readB(float *buffer, long count, std::istream &is) {
    return FileSystem::readB((uint32_t*)buffer, count, is);
  }

  static inline size_t readB(float &f, std::istream &is) {
    return FileSystem::readB((uint32_t*)&f, 1, is);
  }

  static inline size_t readB(long *buffer, long count, std::istream &is) {
    return FileSystem::readB((uint64_t*)buffer, count, is);
  }

  static inline size_t readB(long &i, std::istream &is) {
    return FileSystem::readB((uint64_t*)&i, 1, is);
  }

  static inline size_t readL(short *buffer, long count, std::istream &is) {
    return FileSystem::readL((uint16_t*)buffer, count, is);
  }

  static inline size_t readL(short &i, std::istream &is) {
    return FileSystem::readL((uint16_t*)&i, 1, is);
  }

  static inline size_t readL(int *buffer, long count, std::istream &is) {
    return FileSystem::readL((uint32_t*)buffer, count, is);
  }

  static inline size_t readL(int &i, std::istream &is) {
    return FileSystem::readL((uint32_t*)&i, 1, is);
  }

  static inline size_t readL(float *buffer, long count, std::istream &is) {
    return FileSystem::readL((uint32_t*)buffer, count, is);
  }

  static inline size_t readL(float &f, std::istream &is) {
    return FileSystem::readL((uint32_t*)&f, 1, is);
  }

  static inline size_t readL(long *buffer, long count, std::istream &is) {
    return FileSystem::readL((uint64_t*)buffer, count, is);
  }

  static inline size_t readL(long &i, std::istream &is) {
    return FileSystem::readL((uint64_t*)&i, 1, is);
  }

  static inline std::string readStr(size_t count, std::istream &is) {
    FileData buffer(count+1);
    is.read(&buffer[0], count);
    buffer[count] = '\0';
    return &buffer[0];
  }

  static inline FileData readData(size_t size, std::istream &is) {
    FileData buffer(size);
    is.read(&buffer[0], size);
    return buffer;
  }

  static inline short readShortL(std::istream &is) {
    short value;
    readL(value, is);
    return value;
  }

  static inline int readIntL(std::istream &is) {
    int value;
    readL(value, is);
    return value;
  }
  
  static inline long readLongL(std::istream &is) {
    long value;
    readL(value, is);
    return value;
  }
}

#endif /* FileSystem_h */
