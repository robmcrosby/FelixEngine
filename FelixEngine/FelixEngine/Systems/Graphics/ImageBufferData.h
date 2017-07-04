//
//  ImageBufferData.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ImageBufferData_h
#define ImageBufferData_h

#include "GraphicResources.h"
#include <vector>


namespace fx {
  typedef std::vector<RGBA> ImageBuffer;
  
  struct ImageBufferData {
    ImageBuffer buffer;
    ivec2 size;
  };
}


#endif /* ImageBufferData_h */
