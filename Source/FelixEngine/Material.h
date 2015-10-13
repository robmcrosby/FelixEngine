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
    const Shader* shader() const {return mShader;}
    
    //TextureMap& textureMap() {return mTextureMap;}
    //const TextureMap& textures() const {return mTextureMap;}
    
    UniformMap& uniformMap() {return mUniformMap;}
    const UniformMap& uniforms() const {return mUniformMap;}
    
    Uniform& operator[](const std::string &key) {return mUniformMap[key];}
    
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
    bool applyToTask(GraphicTask &task) const
    {
      if (!mShader)
        return false;
      task.shader = mShader;
      //task.textureMap = &mTextureMap;
      task.materialUniforms = mUniformMap.getInternalMap();
      return true;
    }
    
  private:
    Shader *mShader;
    //TextureMap mTextureMap;
    UniformMap mUniformMap;
    GraphicSystem *mSystem;
  };
}

#endif /* Material_h */
