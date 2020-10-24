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
    std::string _filePath;
    size_t _fileOffset;
    size_t _fileLength;
    
    std::vector<USDItem> _usdData;
  
  public:
    USDCrate();
    ~USDCrate();
    
    bool open(const std::string &filePath, size_t offset = 0, size_t length = 0);
    
  private:
    bool read(std::istream &is);
    StringVector readTokens(long start, std::istream &is);
    RepVector readReps(long start, std::istream &is);
    SpecMap readSpecMap(long start, std::istream &is, IntVector &fieldSets, IntVector &fields, StringVector &tokens, RepVector &reps);
    PathVector readPaths(long start, std::istream &is, StringVector &tokens, SpecMap &specs);
    
    void printUSD();
    
    IntVector readIntVector(long start, std::istream &is);
    IntVector decompressIntVector(long numItems, std::istream &is);
  };

}

#endif /* USDCrate_h */
