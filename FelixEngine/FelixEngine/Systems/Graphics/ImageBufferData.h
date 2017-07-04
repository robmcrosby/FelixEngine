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
    
    void resize(int width, int height) {
      size = ivec2(width, height);
      buffer.resize(width*height);
    }
    
    void* ptr() {return (void*)&buffer.at(0);}
    const void* ptr() const {return (void*)&buffer.at(0);}
  };
}


#endif /* ImageBufferData_h */
