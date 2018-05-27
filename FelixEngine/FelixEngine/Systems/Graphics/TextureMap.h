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
    TextureBufferPtr buffer;
    SamplerState sampler;
    
    Texture(TextureBufferPtr b, SamplerState s): buffer(b), sampler(s) {}
  };
  
  class TextureMap;
  typedef std::shared_ptr<TextureMap> TexturesPtr;
  
  class TextureMap {
  private:
    std::vector<Texture> _textures;
    BufferPoolPtr _bufferPool;
    
  public:
    static TexturesPtr make() {return std::make_shared<TextureMap>();}
    
    TextureMap() {_bufferPool = Graphics::getInstance().createBufferPool();}
    ~TextureMap() {}
    
    std::vector<Texture>::iterator begin() {return _textures.begin();}
    std::vector<Texture>::iterator end() {return _textures.end();}
    
    std::vector<Texture>::const_iterator begin() const {return _textures.begin();}
    std::vector<Texture>::const_iterator end() const {return _textures.end();}
    
    bool loadXML(const XMLTree::Node &node) {
      bool success = true;
      for (auto &subNode : node)
        success &= addTexture(*subNode);
      return success;
    }
    
    bool addTexture(const XMLTree::Node &node) {
      bool success = true;
      TextureBufferPtr texture;
      if (node.hasAttribute("name")) {
        texture = Graphics::getInstance().getTextureBuffer(node.attribute("name"));
        if (node.hasAttribute("file"))
          success = texture->loadXML(node);
      }
      else {
        texture = Graphics::getInstance().createTextureBuffer();
        success = texture->loadXML(node);
      }
      addTexture(texture, node);
      return success;
    }
    
    bool addTexture(const ImageBufferData &image, SamplerState sampler = SamplerState()) {
      Texture texture = Texture(Graphics::getInstance().createTextureBuffer(), sampler);
      bool loaded = texture.buffer->load(image);
      _textures.push_back(texture);
      return loaded;
    }
    
    void addTexture(TextureBufferPtr buffer, SamplerState sampler = SamplerState()) {
      Texture texture = Texture(buffer, sampler);
      _textures.push_back(texture);
    }
  };
}

#endif /* TextureMap_h */
