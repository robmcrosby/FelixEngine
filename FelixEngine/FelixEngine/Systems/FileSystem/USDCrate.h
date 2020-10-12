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
  };

}

#endif /* USDCrate_h */
