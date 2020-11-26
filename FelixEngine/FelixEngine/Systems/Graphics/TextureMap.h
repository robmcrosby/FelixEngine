//
//  TextureMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef TextureMap_h
#define TextureMap_h

#include <FelixEngine/Graphics.h>
#include <FelixEngine/GraphicResources.h>
#include <vector>


namespace fx {
  struct Texture {
    TextureBufferPtr buffer;
    SamplerState sampler;
    
    Texture() {}
    Texture(const Texture &that): buffer(that.buffer), sampler(that.sampler) {}
    Texture(TextureBufferPtr b, SamplerState s): buffer(b), sampler(s) {}
  };
  
  class TextureMap;
  typedef std::shared_ptr<TextureMap> TexturesPtr;
  
  class TextureMap {
  private:
    std::map<std::string, Texture> _map;
    BufferPoolPtr _bufferPool;
    
  public:
    static TexturesPtr make() {return std::make_shared<TextureMap>();}
    
    TextureMap() {_bufferPool = Graphics::getInstance().createBufferPool();}
    ~TextureMap() {}
    
    std::map<std::string, Texture>::iterator begin() {return _map.begin();}
    std::map<std::string, Texture>::iterator end() {return _map.end();}
    
    std::map<std::string, Texture>::const_iterator begin() const {return _map.begin();}
    std::map<std::string, Texture>::const_iterator end() const {return _map.end();}
    
    size_t size() const {return _map.size();}
    
    bool load(const XMLTree::Node &node) {
      bool success = true;
      for (auto &subNode : node)
        success &= setTexture(*subNode);
      return success;
    }
    
    bool setColor(const std::string &name, const RGBA &color) {
      Texture texture = Texture(Graphics::getInstance().createTextureBuffer(), SamplerState());
      _map[name] = texture;
      return texture.buffer->loadColor(color);
    }
    
    bool setTexture(const XMLTree::Node &node) {
      bool success = true;
      if (node.hasAttribute("name")) {
        Texture texture;
        texture.sampler = node;
        if (node.hasAttribute("texture"))
          texture.buffer = Graphics::getInstance().getTextureBuffer(node.attribute("texture"));
        else
          texture.buffer = Graphics::getInstance().createTextureBuffer();
        if (node.hasAttribute("file"))
          success = texture.buffer->loadImageFile(node.attribute("file"));
        _map[node.attribute("name")] = texture;
      }
      else {
        std::cerr << "Texture Needs Name" << std::endl;
        success = false;
      }
      return success;
    }
    
    bool setTexture2D(const std::string &name, const ImageBufferData &image, SamplerState sampler = SamplerState()) {
      Texture texture = Texture(Graphics::getInstance().createTextureBuffer(), sampler);
      _map[name] = texture;
      return texture.buffer->loadImage(image);
    }
    
    bool setTextureCubeMap(const std::string &name, const ImageBufferSet &images, SamplerState sampler = SamplerState()) {
      Texture texture = Texture(Graphics::getInstance().createTextureBuffer(), sampler);
      _map[name] = texture;
      return texture.buffer->loadCubeMap(images);
    }
    
    void setTexture(const std::string &name, TextureBufferPtr buffer, SamplerState sampler = SamplerState()) {
      _map[name] = Texture(buffer, sampler);
    }
    
    bool loaded() const {
      for (auto &texture : _map) {
        if (!texture.second.buffer || !texture.second.buffer->loaded())
          return false;
      }
      return true;
    }
    
    Texture& operator[](const std::string name) {
      return _map[name];
    }
    
    BufferPoolPtr bufferPool() {return _bufferPool;}
  };
}

#endif /* TextureMap_h */
