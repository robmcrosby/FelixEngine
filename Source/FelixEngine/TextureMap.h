//
//  TextureMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TextureMap_h
#define TextureMap_h

#include "FelixEngine.h"
#include "IndexedMap.h"
#include "BufferMap.h"
#include "Sampler.h"
#include "Scene.h"
#include "OwnPtr.h"

namespace fx
{
  /**
   *
   */
  class TextureState
  {
  public:
    TextureState(Scene *scene = nullptr): mScene(scene), mTexture(0) {}
    TextureState(const TextureState &other): mScene(0), mTexture(0) {*this = other;}
    
    TextureState& operator=(const TextureState &other)
    {
      mScene   = other.mScene;
      mSampler = other.mSampler;
      mTexture = other.mTexture;
      return *this;
    }
    
    void setToXml(const XMLTree::Node &node)
    {
      mSampler.setToXml(node);
      setTexture(node);
    }
    
    void setTexture(const BufferMap &texture) {mTexture = texture;}
    void setTexture(BufferMap *texture) {mTexture = texture;}
    void setTexture(const std::string &name)
    {
      if (!mScene || name == "")
        setTexture(&mScene->getBufferMap(name));
      else
        setTexture(BUFFER_MAP_TEXTURE);
    }
    void setTexture(const XMLTree::Node &node)
    {
      setTexture(node.attribute("name"));
      mTexture->setToXml(node);
    }
    BufferMap& texture() {return *mTexture;}
    
    void setSampler(const Sampler &sampler) {mSampler = sampler;}
    const Sampler& sampler() const {return mSampler;}
    
  private:
    Scene *mScene;
    Sampler mSampler;
    OwnPtr<BufferMap> mTexture;
  };
  
  /**
   *
   */
  class TextureMap
  {
  public:
    TextureMap(Scene *scene): mScene(scene), mGraphicSystem(FelixEngine::GetGraphicSystem()) {}
    TextureMap(const TextureMap &other): mScene(other.mScene), mGraphicSystem(FelixEngine::GetGraphicSystem()) {*this = other;}
    virtual ~TextureMap() {}
    
    TextureMap& operator=(const TextureMap &other)
    {
      mTextures.clear();
      for (TextureState &texState : mTextures)
        mTextures.push_back(texState);
      return *this;
    }
    
    void setToXml(const XMLTree::Node &node)
    {
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
        addTexture(**itr);
    }
    
    void addTexture(const BufferMap &texture = BUFFER_MAP_TEXTURE, Sampler sampler = Sampler())
    {
      mTextures.push_back(mScene);
      mTextures.back().setTexture(texture);
      mTextures.back().setSampler(sampler);
    }
    void addTexture(BufferMap *texture, Sampler sampler = Sampler())
    {
      mTextures.push_back(mScene);
      mTextures.back().setTexture(texture);
      mTextures.back().setSampler(sampler);
    }
    void addTexture(const XMLTree::Node &node)
    {
      mTextures.push_back(mScene);
      mTextures.back().setToXml(node);
      mGraphicSystem->uploadBuffer(mTextures.back().texture());
    }
    
    size_t size() const {return mTextures.size();}
    void resize(size_t size)
    {
      if (size < mTextures.size())
        mTextures.resize(size);
      while (mTextures.size() < size)
        mTextures.push_back(mScene);
    }
    void clearTextures() {mTextures.clear();}
    
    TextureState& operator[](int index) {return mTextures.at(index);}
    const TextureState& operator[](int index) const {return mTextures.at(index);}
    
    typedef std::vector<TextureState>::iterator iterator;
    iterator begin() {return mTextures.begin();}
    iterator end() {return mTextures.end();}
    
    typedef std::vector<TextureState>::const_iterator const_iterator;
    const_iterator begin() const {return mTextures.begin();}
    const_iterator end() const {return mTextures.end();}
    
  private:
    Scene *mScene;
    GraphicSystem *mGraphicSystem;
    std::vector<TextureState> mTextures;
  };
}

#endif /* TextureMap_h */
