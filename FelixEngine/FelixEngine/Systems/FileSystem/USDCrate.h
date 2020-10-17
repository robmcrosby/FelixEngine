//
//  USDCrate.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef USDCrate_h
#define USDCrate_h

#include "FileSystem.h"
#include <sstream>


namespace fx {

  struct Path {
    int path;
    int jump;
    bool leaf;
    std::string token;
  };
  typedef std::vector<Path> PathVector;
  
  struct Spec {
    int fset;
    int type;
  };
  typedef std::map<int, Spec> SpecMap;

  /** USDArchive  */
  class USDCrate {
  private:
    std::string _filePath;
    size_t _fileOffset;
    size_t _fileLength;
  
  public:
    USDCrate();
    ~USDCrate();
    
    bool open(const std::string &filePath, size_t offset = 0, size_t length = 0);
    
  private:
    bool read(std::istream &is);
    StringVector readTokens(long start, std::istream &is);
    LongVector readReps(long start, std::istream &is);
    PathVector readPaths(long start, std::istream &is, StringVector &tokens);
    SpecMap readSpecMap(long start, std::istream &is);
    
    
    IntVector readIntVector(long start, std::istream &is);
    IntVector decompressIntVector(long numItems, std::istream &is);
  };

}

#endif /* USDCrate_h */
