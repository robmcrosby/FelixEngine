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
#include "Scene.h"
#include "Buffer.h"
#include "OwnPtr.h"
#include "XMLTree.h"


namespace fx
{
  class Material
  {
  public:
    Material(Scene *scene): mScene(scene) {}
    virtual ~Material() {}
    
    void setToXml(const XMLTree::Node &node)
    {
      if (node.hasAttribute("shader"))
        setShader(node.attribute("shader"));
      
      for (const auto &subNode : node)
      {
        if (*subNode == "Shader")
          setShader(*subNode);
        else if (*subNode == "Textures")
          addTextures(*subNode);
        else if (*subNode == "Texture")
          addTexture(*subNode);
        else
          setUniform(*subNode);
      }
    }
    
    void setShader(const Buffer &shader) {mShader = shader;}
    void setShader(const std::string &name) {mShader = &mScene->getShaderBuffer(name);}
    void setShader(const XMLTree::Node &node) {mShader = &mScene->createShader(node);}
    Buffer& shader() {return *mShader;}
    const Buffer& shader() const {return *mShader;}
    
    
    void setUniforms(const Buffer &uniforms) {mUniforms = uniforms;}
    void setUniforms(const XMLTree::Node &node)
    {
      mUniforms = BUFFER_UNIFORM;
      mUniforms->setToXml(node);
      mUniforms->setToUpdate();
    }
    Buffer& uniforms()
    {
      if (!mUniforms.ptr())
        mUniforms = BUFFER_UNIFORM;
      return *mUniforms;
    }
    const Buffer& uniforms() const {return *mUniforms;}
    
    void setUniform(const XMLTree::Node &node) {uniforms().set(node);}
    void setUniform(const std::string &name, const Variant &var) {uniforms().set(name, var);}
    void setStruct(const std::string &name, const std::string &comp, const Variant &var)
    {
      uniforms().getBuffer(name, BUFFER_STRUCT).set(comp, var);
      uniforms().setToUpdate();
    }
    
    
    void addTexture(const Buffer &texture)
    {
      if (!mTextures.ptr())
        mTextures = BUFFER_TEXTURES;
      mTextures->addBuffer(texture);
    }
    void addTexture(const Buffer &texture, Sampler sampler)
    {
      addTexture(texture);
      mTextures->back().setFlags(sampler.flags());
    }
    void addTexture(const std::string &name) {addTexture(mScene->getTextureBuffer(name));}
    void addTexture(const std::string &name, Sampler sampler)
    {
      addTexture(name);
      mTextures->back().setFlags(sampler.flags());
    }
    void addTexture(const XMLTree::Node &node)
    {
      if (node.hasAttribute("file"))
        addTexture(mScene->createTexture(node), Sampler(node));
      else
        addTexture(node.attribute("name"), Sampler(node));
    }
    void addTextures(const XMLTree::Node &node)
    {
      for (const auto subNode : node)
        addTexture(*subNode);
    }
    
  private:
    OwnPtr<Buffer> mShader;
    OwnPtr<Buffer> mUniforms;
    OwnPtr<Buffer> mTextures;
    
    Scene *mScene;
  };
}

#endif /* Material_h */
