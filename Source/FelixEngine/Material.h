//
//  Material.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/12/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Material_h
#define Material_h

#include "FelixEngine.h"
#include "UniformMap.h"
#include "GraphicTask.h"


namespace fx
{
  class Material
  {
  public:
    Material(): mShader(0), mSystem(FelixEngine::GetGraphicSystem()) {}
    virtual ~Material() {}
    
    void setShader(Shader *shader) {mShader = shader;}
    void setShader(const std::string &name)
    {
      if (mSystem)
        setShader(mSystem->getShader(name));
    }
    
    Shader* shader() {return mShader;}
    const Shader* shader() const {return mShader;}
    
    UniformMap& uniformMap() {return mUniformMap;}
    //TextureMap& textureMap() {return mTextureMap;}
    
    const UniformMap& uniforms() const {return mUniformMap;}
    //const TextureMap& textures() const {return mTextureMap;}
    
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = true;
        
      if (node.hasAttribute("shader"))
        setShader(node.attribute("shader"));
      if (node.hasSubNode("UniformMap"))
        success &= mUniformMap.setToXml(*node.subNode("UniformMap"));
      //if (node.hasSubNode("TextureMap"))
      //  success &= mTextureMap.setToXml(*node.subNode("TextureMap"));

      return success;
    }
    
  private:
    Shader *mShader;
    UniformMap mUniformMap;
    //TextureMap mTextureMap;
    GraphicSystem *mSystem;
  };
}

#endif /* Material_h */
