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

  struct USDTocSection {
    long start;
    long size;
  };
  typedef std::map<std::string, USDTocSection> USDTableOfContents;
  typedef std::vector<std::string> StringVector;

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
    
    IntVector readIntVector(long start, std::istream &is);
  };

}

#endif /* USDCrate_h */
