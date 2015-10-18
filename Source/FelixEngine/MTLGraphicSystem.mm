//
//  MTLGraphicSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "MTLGraphicSystem.h"
#include <SDL2/SDL.h>


namespace fx
{
  
  class MTLWindowInterface: public Window
  {
  public:
    MTLWindowInterface(MTLGraphicSystem *system): mSystem(system) {}
    virtual ~MTLWindowInterface() {}
    
    void update() {}
    
  private:
    MTLGraphicSystem *mSystem;
  };
  
  
  
  class MTLFrameInterface: public Frame
  {
  public:
    MTLFrameInterface(MTLGraphicSystem *system): mSystem(system) {}
    virtual ~MTLFrameInterface() {}
    
    void update() {}
    
  private:
    MTLGraphicSystem *mSystem;
  };
  
  
  
  class MTLShaderInterface: public Shader
  {
  public:
    MTLShaderInterface(MTLGraphicSystem *system): mSystem(system) {}
    virtual ~MTLShaderInterface() {}
    
    void update() {}
    
  private:
    MTLGraphicSystem *mSystem;
  };
  
  
  
  class MTLMeshInterface: public Mesh
  {
  public:
    MTLMeshInterface(MTLGraphicSystem *system): mSystem(system) {}
    virtual ~MTLMeshInterface() {}
    
    void update() {}
    
  private:
    MTLGraphicSystem *mSystem;
  };
  
  
  
  class MTLTextureInterface: public Texture
  {
  public:
    MTLTextureInterface(MTLGraphicSystem *system): mSystem(system) {}
    virtual ~MTLTextureInterface() {}
    
    void update() {}
    
  private:
    MTLGraphicSystem *mSystem;
  };
}


using namespace fx;
using namespace std;


DEFINE_SYSTEM_ID(MTLGraphicSystem)


MTLGraphicSystem::MTLGraphicSystem()
{
  mInitFlags |= SDL_INIT_VIDEO;
}

MTLGraphicSystem::~MTLGraphicSystem()
{
}

Window* MTLGraphicSystem::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
    mWindows[name] = new MTLWindowInterface(this);
  return mWindows[name];
}

Frame* MTLGraphicSystem::getFrame(const std::string &name)
{
  if (!mFrames.count(name))
    mFrames[name] = new MTLFrameInterface(this);
  return mFrames[name];
}

Shader* MTLGraphicSystem::getShader(const std::string &name)
{
  if (!mShaders.count(name))
    mShaders[name] = new MTLShaderInterface(this);
  return mShaders[name];
}

Mesh* MTLGraphicSystem::getMesh(const std::string &name)
{
  if (!mMeshes.count(name))
    mMeshes[name] = new MTLMeshInterface(this);
  return mMeshes[name];
}

Texture* MTLGraphicSystem::getTexture(const std::string &name)
{
  if (!mTextures.count(name))
    mTextures[name] = new MTLTextureInterface(this);
  return mTextures[name];
}

bool MTLGraphicSystem::setToXml(const XMLTree::Node *node)
{
//  if (node)
//  {
//    setVersion(node->subNode("Version"));
//    return addWindows(node->subNode("Windows"));
//  }
//  cerr << "Error: GLGraphicSystem passed a NULL node." << endl;
//  return false;
  
  bool success = true;
  return success;
}

bool MTLGraphicSystem::init()
{
  bool success = true;
  return success;
}

void MTLGraphicSystem::update()
{
//  updateResources();
//  processTasks();
//  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
//    itr->second->swapBuffers();
}

InternalUniformMap* MTLGraphicSystem::createUniformMap()
{
  return nullptr;
}

void MTLGraphicSystem::processTasks()
{
  clearTaskSlots();
  loadTaskSlots();
  
  if (mTaskSlots.size())
    processTaskSlot(mTaskSlots.at(0));
}

void MTLGraphicSystem::processTaskSlot(const TaskSlot &slot)
{
  for (TaskSlot::const_iterator itr = slot.begin(); itr != slot.end(); ++itr)
    processTask(*itr);
}

void MTLGraphicSystem::processTask(const GraphicTask &task)
{
//  if (task.isViewTask() && task.viewIndex < (int)mTaskSlots.size())
//  {
//    const TaskSlot &slot = mTaskSlots.at(task.viewIndex);
//    if (slot.size())
//    {
//      const GLFrame *frame = static_cast<const GLFrame*>(task.frame);
//      frame->use();
//      if (task.isClearTask())
//        frame->clear(task.clearState);
//      processTaskSlot(slot);
//    }
//  }
//  else if (task.isDrawTask())
//  {
//    const GLFrame  *frame  = static_cast<const GLFrame*>(task.frame);
//    const GLShader *shader = static_cast<const GLShader*>(task.shader);
//    const GLMesh   *mesh   = static_cast<const GLMesh*>(task.mesh);
//    
//    // Set the state for the Frame
//    frame->use();
//    if (task.isClearTask())
//      frame->clear(task.clearState);
//    frame->setDepthState(task.depthState);
//    frame->setBlendState(task.blendState);
//    
//    // Set the state for the Shader
//    shader->use();
//    if (task.viewUniforms)
//      static_cast<const GLUniformMap*>(task.viewUniforms)->applyToShader(shader);
//    if (task.materialUniforms)
//      static_cast<const GLUniformMap*>(task.materialUniforms)->applyToShader(shader);
//    if (task.localUniforms)
//      static_cast<const GLUniformMap*>(task.localUniforms)->applyToShader(shader);
//    shader->applyTextureMap(task.textureMap);
//    
//    // Draw the Mesh
//    mesh->draw(shader, task.instances, task.subMesh);
//  }
}

void MTLGraphicSystem::updateResources()
{
  for (std::map<std::string, MTLWindowInterface*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLFrameInterface*>::iterator itr = mFrames.begin(); itr != mFrames.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLShaderInterface*>::iterator itr = mShaders.begin(); itr != mShaders.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLMeshInterface*>::iterator itr = mMeshes.begin(); itr != mMeshes.end(); ++itr)
    itr->second->update();
  for (std::map<std::string, MTLTextureInterface*>::iterator itr = mTextures.begin(); itr != mTextures.end(); ++itr)
    itr->second->update();
}