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
  struct Texture {
    TextureBuffer *buffer;
    SamplerState sampler;
    
    Texture(TextureBuffer *b, SamplerState s): buffer(b), sampler(s) {}
  };
  
  class TextureMap {
  private:
    std::vector<Texture> _textures;
    
  public:
    TextureMap() {}
    ~TextureMap() {}
    
    std::vector<Texture>::iterator begin() {return _textures.begin();}
    std::vector<Texture>::iterator end() {return _textures.end();}
    
    std::vector<Texture>::const_iterator begin() const {return _textures.begin();}
    std::vector<Texture>::const_iterator end() const {return _textures.end();}
    
    bool addTexture(const ImageBufferData &image, SamplerState sampler = SamplerState()) {
      Texture texture = Texture(Graphics::getInstance().createTextureBuffer(), sampler);
      bool loaded = texture.buffer->load(image);
      _textures.push_back(texture);
      return loaded;
    }
    
    void addTexture(TextureBuffer *buffer, SamplerState sampler = SamplerState()) {
      Texture texture = Texture(buffer, sampler);
      _textures.push_back(texture);
    }
  };
}

#endif /* TextureMap_h */
