//
//  TextureMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TextureMap_h
#define TextureMap_h

#include "IndexedMap.h"
#include "FelixEngine.h"
#include "GraphicSystem.h"
#include "GraphicResources.h"
#include <vector>

namespace fx
{
  /**
   *
   */
  class TextureState
  {
  public:
    TextureState(): mTexture(0) {}
    TextureState(const std::string &name) {setTexture(name);}
    
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = false;
      if (node.hasAttribute("name"))
      {
        setTexture(node.attribute("name"));
        success = mTexture && mSampler.setToXml(node);
        
        // Check if the node needs to be set to the texture
        if (mTexture && node.hasAttribute("file"))
          success &= mTexture->setToXml(node);
      }
      return success;
    }
    
    void setTexture(Texture *texture) {mTexture = texture;}
    void setTexture(const std::string &name)
    {
      GraphicSystem *system = FelixEngine::GetGraphicSystem();
      if (system)
        setTexture(system->getTexture(name));
    }
    Texture* texture() const {return mTexture;}
    
    void setSampler(Sampler sampler) {mSampler = sampler;}
    Sampler& sampler() {return mSampler;}
    const Sampler& sampler() const {return mSampler;}
    
  private:
    Texture *mTexture;
    Sampler  mSampler;
  };
  
  /**
   *
   */
  class TextureMap: public IndexedMap<TextureState>
  {
  public:
    typedef IndexedMap<TextureState>::iterator iterator;
    typedef IndexedMap<TextureState>::const_iterator const_iterator;
    
  public:
    TextureMap() {}
    virtual ~TextureMap() {}
    
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = true;
      
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        if ((*itr)->hasAttribute("name"))
        {
          push((*itr)->attribute("name"));
          success &= back().setToXml(**itr);
        }
        else
          success = false;
      }
      return success;
    }
  };
}


#endif /* TextureMap_h */
