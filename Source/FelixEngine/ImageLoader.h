//
//  ImageLoader.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef ImageLoader_h
#define ImageLoader_h

#include "Image.h"
#include "BufferMap.h"


namespace fx
{
  /**
   *
   */
  class ImageLoader
  {
  public:
    static bool LoadImageFromFile(ImageRGBA &image, const std::string &file);
    static bool LoadImageFromFile(BufferMap &textureMap, const std::string &file);
  };
}

#endif /* ImageLoader_h */
