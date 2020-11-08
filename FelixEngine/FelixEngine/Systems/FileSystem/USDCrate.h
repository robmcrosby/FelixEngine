//
//  USDCrate.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef USDCrate_h
#define USDCrate_h

#include "USDTypes.h"
#include <sstream>


namespace fx {
  /** USDArchive  */
  class USDCrate {
  private:
    friend USDItem;
    
    std::string _filePath;
    size_t _fileOffset;
    size_t _fileLength;
    int _fileVersion;
    mutable std::ifstream *_fileStream;
    
    int _rootItem;
    std::vector<USDItem> _usdItems;
    StringMap _pathMap;
    
    StringVector _objects;
    StringVector _meshes;
    StringVector _materials;
  
  public:
    USDCrate();
    ~USDCrate();
    
    bool open(const std::string &filePath, size_t offset = 0, size_t length = 0);
    bool open() const;
    void close() const;
    
    const StringVector& objectPaths() const {return _objects;}
    const StringVector& meshPaths() const {return _meshes;}
    const StringVector& materialPaths() const {return _materials;}
    
    std::string getName(const std::string &path) const;
    std::string getParentPath(const std::string &path) const;
    
    bool hasAttribute(const std::string &itemPath, const std::string &attribute) const;
    bool hasInput(const std::string &itemPath, const std::string &name) const;
    
    bool isTextureInput(const std::string &itemPath, const std::string &name) const;
    
    std::string getAttributePath(const std::string &itemPath, const std::string &attribute) const;
    std::string getInputPath(const std::string &itemPath, const std::string &name) const;
    std::string getTexturePath(const std::string &itemPath, const std::string &name) const;
    std::string getShaderPath(const std::string &materialPath) const;
    
    std::string getFirstMeshPath(const std::string &path) const;
    bool getMeshPaths(StringVector &paths, const std::string &path) const;
    
    std::string getMaterialPath(const std::string &path) const;
    
    bool getArray(IntVector &buffer, const std::string &itemPath, const std::string &attribute) const;
    bool getArray(std::vector<vec2> &buffer, const std::string &itemPath, const std::string &attribute) const;
    bool getArray(std::vector<vec3> &buffer, const std::string &itemPath, const std::string &attribute) const;
    bool appendBuffer(VertexBuffer &buffer, const std::string &itemPath, const std::string &attribute) const;
    
    bool getFaceCounts(IntVector &buffer, const std::string &meshPath) const;
    bool getFaceIndices(IntVector &buffer, const std::string &meshPath) const;
    bool getPoints(std::vector<vec3> &buffer, const std::string &meshPath) const;
    
    bool getNormalIndices(IntVector &buffer, const std::string &meshPath) const;
    bool getNormals(std::vector<vec3> &buffer, const std::string &meshPath) const;
    
    StringVector getUVNames(const std::string &meshPath) const;
    bool getUVIndices(IntVector &buffer, const std::string &meshPath, const std::string &name) const;
    bool getUVs(std::vector<vec2> &buffer, const std::string &meshPath, const std::string &name) const;
    
    void printUSD() const;
    
  private:
    int findAttributeIndex(const std::string &itemPath, const std::string &name) const;
    
    bool readTableOfContents();
    StringVector readTokens(long start, std::istream &is);
    RepVector readReps(long start, std::istream &is);
    SpecMap readSpecMap(long start, std::istream &is, IntVector &fieldSets, IntVector &fields, StringVector &tokens, RepVector &reps);
    PathVector readPaths(long start, std::istream &is, StringVector &tokens, SpecMap &specs);
    
    IntVector readIntVector(long start, std::istream &is);
    IntVector decompressIntVector(long numItems, std::istream &is);
  };

}

#endif /* USDCrate_h */
