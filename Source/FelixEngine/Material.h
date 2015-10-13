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
    
    bool setShader(const XMLTree::Node &node)
    {
      bool success = false;
      if (node.hasAttribute("name"))
      {
        setShader(node.attribute("name"));
        success = mShader->setToXml(node);
      }
      return success;
    }
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
    
    bool setToXml(const XMLTree::Node *node) {return setToXml(*node);}
    bool setToXml(const XMLTree::Node &node)
    {
      bool success = true;
      // Set the Shader
      if (node.hasAttribute("shader"))
        setShader(node.attribute("shader"));
      else if (node.hasSubNode("Shader"))
        success &= setShader(*node.subNode("Shader"));
      
      // Set the UniformMap
      if (node.hasSubNode("UniformMap"))
        success &= mUniformMap.setToXml(*node.subNode("UniformMap"));
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
