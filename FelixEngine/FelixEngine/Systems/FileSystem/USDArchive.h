//
//  USDArchive.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef USDArchive_h
#define USDArchive_h

#include "FileSystem.h"
#include <sstream>


namespace fx {
  
  struct USDEntry {
    std::string name;
    size_t offset;
  };
  typedef std::vector<USDEntry> USDEntryList;
  
  /** USDArchive  */
  class USDArchive {
  private:
    std::string _filePath;
    USDEntryList _entries;
    
  public:
    USDArchive();
    ~USDArchive();
    
    bool open(const std::string &filePath);
    
  private:
    bool read(std::istream &is);
    bool findNextSignature(std::istream &is, int signature);
    bool findPrevSignature(std::istream &is, int signature);
  };
}

#endif /* USDArchive_h */
