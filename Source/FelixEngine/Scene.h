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

#include <istream>
#include <list>

namespace fx
{
  class Scene: public Component
  {
  public:
    Scene(): Component(this),
      mDirectory(FileSystem::GetResources()),
      mGraphicSystem(FelixEngine::Instance()->getGraphicSystem()),
      mBufferMaps()
    {
      mScene = this;
      setEventFlags(EVENT_APP_UPDATE);
    }
    ~Scene()
    {
      clearMaterials();
      clearBuffers();
      
      clearBufferMaps();
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
    
    Buffer& getBufferMap(const std::string name)
    {
      if (!mBufferMaps.count(name))
      {
        mBufferMaps[name] = new Buffer();
        mBufferMaps[name]->setName(name);
      }
      return *mBufferMaps[name];
    }
    void clearBufferMaps()
    {
      for (BufferMapDirectory::iterator itr = mBufferMaps.begin(); itr != mBufferMaps.end(); ++itr)
        delete itr->second;
      mBufferMaps.clear();
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
    
    BufferMapDirectory mBufferMaps;
    GraphicSystem *mGraphicSystem;
  };
}

#endif /* Scene_h */
