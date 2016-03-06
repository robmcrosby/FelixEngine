//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h


#include "Component.h"
#include "XMLTree.h"
#include "Material.h"
#include "Buffer.h"

#include "GraphicSystem.h"
#include "MeshLoader.h"
#include "TextureLoader.h"

#include <istream>
#include <list>

namespace fx
{
  class Scene: public Component
  {
  public:
    Scene(): Component(this),
      mDirectory(FileSystem::GetResources()),
      mGraphicSystem(FelixEngine::Instance()->getGraphicSystem())
    {
      mScene = this;
      setEventFlags(EVENT_APP_UPDATE);
    }
    ~Scene()
    {
      clearMaterials();
      clearBuffers();
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_APP_UPDATE)
        update();
    }
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        if (node.tree() && node.tree()->url() != "")
          mDirectory = node.tree()->url();
          addChild(**itr);
      }
    }
    bool init()
    {
      bool success = true;
      for (iterator itr = begin(); itr != end(); ++itr)
        success &= (*itr)->init();
      return success;
    }
    
    
    Material* getMaterial(const std::string &name)
    {
      if (!mMaterialMap.count(name))
        mMaterialMap[name] = new Material();
      return mMaterialMap.at(name);
    }
    void clearMaterials()
    {
      for (std::map<std::string, Material*>::iterator itr = mMaterialMap.begin(); itr != mMaterialMap.end(); ++itr)
        delete itr->second;
      mMaterialMap.clear();
    }
    
    
    Buffer& getFrameBuffer(const std::string name)
    {
      if (!mFrameBuffers.contains(name))
      {
        Frame *frame = mGraphicSystem->getFrame(name);
        Buffer *buffer = new Buffer(frame, BUFFER_FRAME, name);
        mFrameBuffers.push(name, buffer);
      }
      return *mFrameBuffers[name];
    }
    Buffer& createFrame(const XMLTree::Node &node)
    {
      Buffer &frame = getFrameBuffer(node.attribute("name"));
      for (const auto &subNode : node)
      {
        Buffer &buffer = frame.addBuffer(subNode->attribute("name"), BUFFER_TARGET);
        buffer.setFlags(ParseColorType(subNode->attribute("format")));
      }
      if (node.hasAttribute("size"))
        frame.addBuffer("size", ivec2(node.attribute("size")));
      else if (node.hasAttribute("reference"))
      {
        frame.addBuffer("reference", node.attribute("reference"));
        if (node.hasAttribute("scale"))
          frame.addBuffer("scale", vec2(node.attribute("scale")));
      }
      frame.setToUpdate();
      mGraphicSystem->uploadBuffer(frame);
      return frame;
    }

    Buffer& getShaderBuffer(const std::string name)
    {
      if (!mShaderBuffers.contains(name))
      {
        Shader *shader = mGraphicSystem->getShader(name);
        Buffer *buffer = new Buffer(shader, BUFFER_PROGRAM, name);
        mShaderBuffers.push(name, buffer);
      }
      return *mShaderBuffers[name];
    }
    Buffer& createShader(const XMLTree::Node &node)
    {
      Buffer &shader = getShaderBuffer(node.attribute("name"));
      for (const auto &subNode : node)
      {
        Buffer &buffer = shader.addBuffer(subNode->attribute("name"), BUFFER_SHADER);
        buffer = subNode->contents();
        buffer.setFlags((int)Shader::ParseShaderPart(subNode->element()));
      }
      shader.setToUpdate();
      mGraphicSystem->uploadBuffer(shader);
      return shader;
    }

    Buffer& getMeshBuffer(const std::string name)
    {
      if (!mMeshBuffers.contains(name))
      {
        Mesh *mesh = mGraphicSystem->getMesh(name);
        Buffer *buffer = new Buffer(mesh, BUFFER_MESH, name);
        mMeshBuffers.push(name, buffer);
      }
      return *mMeshBuffers[name];
    }
    Buffer& createMesh(const XMLTree::Node &node)
    {
      Buffer &mesh = getMeshBuffer(node.attribute("name"));
      MeshLoader::LoadMeshFromXML(mesh, node);
      mesh.setToUpdate();
      mGraphicSystem->uploadBuffer(mesh);
      return mesh;
    }
    
    Buffer& getTextureBuffer(const std::string name)
    {
      if (!mTextureBuffers.contains(name))
      {
        Texture *texture = mGraphicSystem->getTexture(name);
        Buffer *buffer = new Buffer(texture, BUFFER_TEXTURE, name);
        mTextureBuffers.push(name, buffer);
      }
      return *mTextureBuffers[name];
    }
    Buffer& createTexture(const XMLTree::Node &node)
    {
      Buffer &texture = getTextureBuffer(node.attribute("name"));
      TextureLoader::LoadTextureFromXml(texture, node);
      texture.setToUpdate();
      mGraphicSystem->uploadBuffer(texture);
      return texture;
    }
    
    void clearBuffers()
    {
      for (Buffer *buffer : mFrameBuffers)
        delete buffer;
      mFrameBuffers.clear();
      
      for (Buffer *buffer : mShaderBuffers)
        delete buffer;
      mShaderBuffers.clear();
      
      for (Buffer *buffer : mMeshBuffers)
        delete buffer;
      mMeshBuffers.clear();
      
      for (Buffer *buffer : mTextureBuffers)
        delete buffer;
      mTextureBuffers.clear();
    }
    
    
    int getPassIndex(const std::string &name)
    {
      if (!mPassNameMap.count(name))
        mPassNameMap[name] = (int)mPassNameMap.size()+1;
      return mPassNameMap[name];
    }
    
    File directory() const {return mDirectory;}
    
  private:
    File mDirectory;
    
    IndexedMap<Buffer*> mFrameBuffers;
    IndexedMap<Buffer*> mShaderBuffers;
    IndexedMap<Buffer*> mMeshBuffers;
    IndexedMap<Buffer*> mTextureBuffers;
    
    std::map<std::string, Material*> mMaterialMap;
    
    std::map<std::string, int> mPassNameMap;
    
    GraphicSystem *mGraphicSystem;
  };
}

#endif /* Scene_h */
