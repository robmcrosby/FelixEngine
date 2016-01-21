//
//  TextureLoader.h
//  FelixEngine
//
//  Created by Robert Crosby on 1/19/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#ifndef TextureLoader_h
#define TextureLoader_h

#include "XMLTree.h"

namespace fx
{
  class BufferMap;
  class Buffer;
  
  class TextureLoader
  {
  public:
    static bool LoadTextureFromXml(BufferMap &texture, const XMLTree::Node &node);
    static bool LoadTextureFromFile(BufferMap &texture, const std::string &file);
  };
}

#endif /* TextureLoader_hpp */
