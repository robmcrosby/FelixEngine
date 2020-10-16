//
//  CompressionUtils.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/15/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef CompressionUtils_h
#define CompressionUtils_h

#include "FileSystem.h"


namespace fx {
  
  /** Compression Utils  */
  class CompressionUtils {
  public:
    virtual ~CompressionUtils() {}
    
    static bool decompressLZ4(FileData &dst, std::istream &is, size_t size);
  
  private:
    static bool decompressLZ4Chunk(FileData &dst, std::istream &is, size_t size);
  };

}


#endif /* CompressionUtils_h */
