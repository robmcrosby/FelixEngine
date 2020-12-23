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
#include <FelixEngine/FileSystem.h>
#include <vector>


namespace fx {
  struct Texture {
    TextureBufferPtr buffer;
    SamplerState sampler;
    
    Texture() {}
    Texture(const Texture &that): buffer(that.buffer), sampler(that.sampler) {}
    Texture(TextureBufferPtr b, SamplerState s): buffer(b), sampler(s) {}
    
    void createBuffer() {buffer = Graphics::getInstance().createTextureBuffer();}
    void setBuffer(const std::string &name) {
      if (name == "")
        createBuffer();
      else
        buffer = Graphics::getInstance().getTextureBuffer(name);
    }
    
    bool loadTexture(const XMLTree::Node &node) {
      if (node.hasAttribute("file"))
        return loadTexture(node.attribute("file"), node, node.attribute("texture"));
      sampler = node;
      setBuffer(node.attribute("texture"));
      return true;
    }
    
    bool loadTexture(const std::string &file, SamplerState sampler = SamplerState(), const std::string &name = "") {
      this->sampler = sampler;
      setBuffer(name);
      return buffer->loadImageFile(file, sampler.mipMappingEnabled());
    }
    
    bool loadTexture(const ImageBufferData &image, SamplerState sampler = SamplerState(), const std::string &name = "") {
      this->sampler = sampler;
      return buffer->loadImage(image, sampler.mipMappingEnabled());
    }
    
    bool loadCubeMap(const XMLTree::Node &node) {
      if (node.hasAttribute("file"))
        return loadCubeMap(node.attribute("file"), node, node.attribute("texture"));
      sampler = node;
      setBuffer(node.attribute("texture"));
      return true;
    }
    
    bool loadCubeMap(const std::string &file, SamplerState sampler = SamplerState(), const std::string &name = "") {
      this->sampler = sampler;
      setBuffer(name);
      return buffer->loadCubeMapFile(file, sampler.mipMappingEnabled());
    }
    
    bool loadCubeMap(const std::vector<std::string> &files, SamplerState sampler = SamplerState(), const std::string &name = "") {
      this->sampler = sampler;
      setBuffer(name);
      return buffer->loadCubeMapFiles(files, sampler.mipMappingEnabled());
    }
    
    bool loadCubeMap(const ImageBufferSet &images, SamplerState sampler = SamplerState(), const std::string &name = "") {
      this->sampler = sampler;
      setBuffer(name);
      return buffer->loadCubeMap(images, sampler.mipMappingEnabled());
    }
    
    bool loadCubeMap(const ImageBufferData &image, SamplerState sampler = SamplerState(), const std::string &name = "") {
      this->sampler = sampler;
      setBuffer(name);
      return buffer->loadCubeMap(image, sampler.mipMappingEnabled());
    }
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
      if (node.hasAttribute("name"))
        return _map[node.attribute("name")].loadTexture(node);
      std::cerr << "Texture Needs Name" << std::endl;
      return false;
    }
    
    void setTexture(const std::string &name, TextureBufferPtr buffer, SamplerState sampler = SamplerState()) {
      _map[name] = Texture(buffer, sampler);
    }
    
    bool setTexture(const std::string &name, const ImageBufferData &image, SamplerState sampler = SamplerState()) {
      return _map[name].loadTexture(image, sampler);
    }
    
    bool setTexture(const std::string &name, const std::string &file, SamplerState sampler = SamplerState()) {
      return _map[name].loadTexture(file, sampler);
    }
    
    bool setCubeMap(const XMLTree::Node &node) {
      if (node.hasAttribute("name"))
        return _map[node.attribute("name")].loadCubeMap(node);
      std::cerr << "CubeMap Needs Name" << std::endl;
      return false;
    }
    
    bool setCubeMap(const std::string &name, const std::vector<std::string> &files, SamplerState sampler = SamplerState()) {
      return _map[name].loadCubeMap(files, sampler);
    }
    
    bool setCubeMap(const std::string &name, const std::string &file, SamplerState sampler = SamplerState()) {
      return _map[name].loadCubeMap(file, sampler);
    }
    
    bool setCubeMap(const std::string &name, const ImageBufferSet &images, SamplerState sampler = SamplerState()) {
      return _map[name].loadCubeMap(images, sampler);
    }
    
    bool setCubeMap(const std::string &name, const ImageBufferData &image, SamplerState sampler = SamplerState()) {
      return _map[name].loadCubeMap(image, sampler);
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
