//
//  TextureMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef TextureMap_h
#define TextureMap_h

#include "Graphics.h"
#include "GraphicResources.h"
#include <vector>


namespace fx {
  class TextureMap {
  private:
    std::vector<TextureBuffer*> _textures;
    
  public:
    TextureMap() {}
    ~TextureMap() {}
    
    std::vector<TextureBuffer*>::iterator begin() {return _textures.begin();}
    std::vector<TextureBuffer*>::iterator end() {return _textures.end();}
    
    std::vector<TextureBuffer*>::const_iterator begin() const {return _textures.begin();}
    std::vector<TextureBuffer*>::const_iterator end() const {return _textures.end();}
    
    bool addTexture(const ImageBufferData *image) {
      return false;
    }
  };
}

#endif /* TextureMap_h */
