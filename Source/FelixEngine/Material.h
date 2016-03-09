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
#include "Component.h"
#include "Scene.h"
#include "Buffer.h"
#include "OwnPtr.h"
#include "XMLTree.h"


namespace fx
{
  class Material: public Component
  {
  public:
    Material(Scene *scene): Component(scene),
      mGraphicSystem(FelixEngine::GetGraphicSystem()),
      mShader(0),
      mUniforms(0),
      mTextures(0)
    {
      setEventFlags(EVENT_APP_RENDER);
      mGraphicSystem->addHandler(this);
    }
    Material(const Material &other): Component(other.mScene),
      mGraphicSystem(FelixEngine::GetGraphicSystem()),
      mShader(0),
      mUniforms(0),
      mTextures(0)
    {
      *this = other;
      setEventFlags(EVENT_APP_RENDER);
      mGraphicSystem->addHandler(this);
    }
    virtual ~Material() {}
    
    Material& operator=(const Material &other)
    {
      mShader = other.mShader;
      mUniforms = other.mUniforms;
      mTextures = other.mTextures;
      return *this;
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_APP_RENDER && event.sender() == mGraphicSystem)
        updateBuffers();
    }
    
    virtual void setToXml(const XMLTree::Node &node)
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
    
    Buffer& shader()
    {
      if (!mShader.ptr())
        mShader = BUFFER_PROGRAM;
      return *mShader;
    }
    Buffer* shaderPtr() const {return mShader.ptr();}
    void setShader(const Buffer &shader) {mShader = shader;}
    void setShader(const std::string &name) {mShader = &mScene->getShaderBuffer(name);}
    void setShader(const XMLTree::Node &node) {mShader = &mScene->createShader(node);}
    void clearShader() {mShader.clear();}
    
    
    Buffer& uniforms()
    {
      if (!mUniforms.ptr())
        mUniforms = BUFFER_UNIFORM;
      return *mUniforms;
    }
    Buffer* uniformsPtr() const {return mUniforms.ptr();}
    void setUniforms(const Buffer &uniforms) {mUniforms = uniforms;}
    void setUniforms(const XMLTree::Node &node)
    {
      uniforms().setToXml(node);
      uniforms().setToUpdate();
    }
    
    void setUniform(const XMLTree::Node &node) {uniforms().set(node);}
    void setUniform(const std::string &name, const Variant &var) {uniforms().set(name, var);}
    void setStruct(const std::string &name, const std::string &comp, const Variant &var)
    {
      uniforms().getBuffer(name, BUFFER_STRUCT).set(comp, var);
      uniforms().setToUpdate();
    }
    void clearUniforms() {mUniforms.clear();}
    
    
    Buffer& textures()
    {
      if (!mTextures.ptr())
        mTextures = BUFFER_TEXTURES;
      return *mTextures;
    }
    Buffer* texturesPtr() const {return mTextures.ptr();}
    void addTexture(const Buffer &texture) {textures().addBuffer(texture);}
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
    void clearTextures() {mTextures.clear();}
    
  private:
    void updateBuffers()
    {
      if (mShader.ptr() && !mShader->updated())
        mGraphicSystem->uploadBuffer(*mShader);
      if (mUniforms.ptr() && !mUniforms->updated())
        mGraphicSystem->uploadBuffer(*mUniforms);
    }
    
    OwnPtr<Buffer> mShader;
    OwnPtr<Buffer> mUniforms;
    OwnPtr<Buffer> mTextures;
    
    GraphicSystem *mGraphicSystem;
  };
}

#endif /* Material_h */
