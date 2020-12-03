//
//  ImageBufferData.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ImageBufferData_h
#define ImageBufferData_h

#include <FelixEngine/GraphicResources.h>
#include <vector>


namespace fx {
  typedef std::vector<uint8_t> ImageBuffer;
  
  struct ImageBufferData {
    ImageBuffer buffer;
    ivec2 size;
    int pixelBytes;
    std::string colorSpace;
    
    void resize(int width, int height, int pixel = 4) {
      size = ivec2(width, height);
      pixelBytes = pixel;
      buffer.resize(width*height*pixel);
    }
    
    void* ptr() {return (void*)&buffer.at(0);}
    const void* ptr() const {return (void*)&buffer.at(0);}
    
    size_t sizeInBytes() const {return buffer.size();}
    
    RGBA& operator[](int i) {return (RGBA&)buffer[i*4];}
  };
  typedef std::vector<ImageBufferData> ImageBufferSet;
}

#endif /* ImageBufferData_h */
