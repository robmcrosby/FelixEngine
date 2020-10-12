//
//  USDArchive.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef USDArchive_h
#define USDArchive_h

#include "FileSystem.h"
#include "USDCrate.h"
#include <sstream>


namespace fx {
  
  /** USDEntry */
  struct USDEntry {
    std::string name;
    size_t headerOffset;
    size_t fileOffset;
    size_t fileLength;
  };
  typedef std::vector<USDEntry> USDEntryList;
  typedef std::vector<std::string> FileList;
  
  /** USDArchive  */
  class USDArchive {
  private:
    std::string _filePath;
    USDEntryList _entries;
    
  public:
    USDArchive();
    ~USDArchive();
    
    bool open(const std::string &filePath);
    
    FileList allFiles() const;
    FileList crateFiles() const;
    FileList imageFiles() const;
    
    USDEntry getEntry(const std::string &name) const;
    
    USDCrate getUSDCrate(const std::string &name) const;
    USDCrate getFirstUSDCrate() const;
    
  private:
    bool read(std::istream &is);
    bool findNextSignature(std::istream &is, int signature);
    bool findPrevSignature(std::istream &is, int signature);
  };
}

#endif /* USDArchive_h */
