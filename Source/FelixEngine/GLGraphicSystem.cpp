//
//  GLGraphicSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLGraphicSystem.h"
#include "GLUniformMap.h"

#include "GLWindow.h"
#include "GLFrame.h"
#include "GLShader.h"
#include "GLMesh.h"
#include "GLTexture.h"


using namespace fx;
using namespace std;


DEFINE_SYSTEM_ID(GLGraphicSystem)


GLGraphicSystem::GLGraphicSystem(): mContext(0)
{
  mSDLInitFlags |= SDL_INIT_VIDEO;
  //mUpdateDelegate = UpdateDelegate::Create<GLGraphicSystem, &GLGraphicSystem::update>(this);
}

GLGraphicSystem::~GLGraphicSystem()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    delete itr->second;
}

Window* GLGraphicSystem::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
  {
    GLWindow *window = new GLWindow(this);
    mWindows[name] = window;
    
    GLFrame *frame = static_cast<GLFrame*>(getFrame(name));
    frame->retain();
    frame->setToWindow(window);
  }
  return mWindows[name];
}

Frame* GLGraphicSystem::getFrame(const std::string &name)
{
  if (!mFrames.count(name))
    mFrames[name] = new GLFrame(this);
  return mFrames[name];
}

Shader* GLGraphicSystem::getShader(const std::string &name)
{
  if (!mShaders.count(name))
    mShaders[name] = new GLShader(this);
  return mShaders[name];
}

Mesh* GLGraphicSystem::getMesh(const std::string &name)
{
  if (!mMeshes.count(name))
    mMeshes[name] = new GLMesh(this);
  return mMeshes[name];
}

Texture* GLGraphicSystem::getTexture(const std::string &name)
{
  if (!mTextures.count(name))
    mTextures[name] = new GLTexture(this);
  return mTextures[name];
}

void GLGraphicSystem::setVersion(int major, int minor)
{
  mMajorVersion = major;
  mMinorVersion = minor;
}

bool GLGraphicSystem::setToXml(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = setVersion(node->subNode("Version"));
    success &= addWindows(node->subNode("Windows"));
    success &= setShaderFunctions(node->subNode("ShaderFunctions"));
  }
  else
    cerr << "Error: GLGraphicSystem passed a NULL node." << endl;
  return success;
}

bool GLGraphicSystem::init()
{
  bool success = true;
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mMajorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mMinorVersion);
  
  // Should be Apple Only.
  if (mMajorVersion > 2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  
  for (map<std::string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    success &= itr->second->init();
  
  cout << glGetString(GL_VERSION) << endl;
  
  return success;
}

bool GLGraphicSystem::setVersion(const XMLTree::Node *node)
{
  if (node && node->hasAttribute("major") && node->hasAttribute("minor"))
  {
    mMajorVersion = node->attributeAsInt("major");
    mMinorVersion = node->attributeAsInt("minor");
    return true;
  }
  cerr << "Warning: Unable to determine OpenGL Version from settings, using 2.1 for now." << endl;
  return false;
}

bool GLGraphicSystem::setShaderFunctions(const XMLTree::Node *node)
{
  bool success = true;
  if (node)
  {
    for (XMLTree::const_iterator itr = node->begin(); itr != node->end(); ++itr)
    {
      string name = (*itr)->attribute("name");
      if (name != "")
        mShaderFunctions[name] = (*itr)->contents();
    }
  }
  return success;
}

void GLGraphicSystem::render()
{
  processTasks();
  
  updateResources();
  updateUniforms();
}


InternalUniformMap* GLGraphicSystem::getInternalUniformMap(UniformMap *map)
{
  GLUniformMap *internalMap = new GLUniformMap(map);
  mGLUniforms.push_back(internalMap);
  return internalMap;
}

void GLGraphicSystem::processTasks()
{
  mTaskSlotsMutex.lock();
  if (mTaskSlots.size())
    processTaskSlot(mTaskSlots.at(0));
  mTaskSlotsMutex.unlock();
  
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->swapBuffers();
}

void GLGraphicSystem::processTaskSlot(const TaskSlot &slot)
{
  for (TaskSlot::const_iterator itr = slot.begin(); itr != slot.end(); ++itr)
    processTask(*itr);
}

void GLGraphicSystem::processTask(const GraphicTask &task)
{
  if (task.isViewTask())
  {
    const GLFrame *frame = static_cast<const GLFrame*>(task.frame);
    frame->use();
    if (task.isClearTask())
      frame->clear(task.clearState);
    
    if (task.viewIndex < (int)mTaskSlots.size())
    {
      const TaskSlot &slot = mTaskSlots.at(task.viewIndex);
      if (slot.size())
        processTaskSlot(slot);
    }
  }
  else if (task.isDrawTask())
  {
    const GLFrame  *frame  = static_cast<const GLFrame*>(task.frame);
    const GLShader *shader = static_cast<const GLShader*>(task.shader);
    const GLMesh   *mesh   = static_cast<const GLMesh*>(task.mesh);
    
    if (frame->isLoaded() && shader->isLoaded() && mesh->isLoaded())
    {
      // Set the state for the Frame
      frame->use();
      if (task.isClearTask())
        frame->clear(task.clearState);
      frame->setDepthState(task.depthState);
      frame->setBlendState(task.blendState);
      
      // Set the state for the Shader
      shader->use();
      if (task.viewUniforms)
        static_cast<const GLUniformMap*>(task.viewUniforms)->applyToShader(shader);
      if (task.materialUniforms)
        static_cast<const GLUniformMap*>(task.materialUniforms)->applyToShader(shader);
      if (task.localUniforms)
        static_cast<const GLUniformMap*>(task.localUniforms)->applyToShader(shader);
      shader->applyTextureMap(task.textureMap);
      
      // Draw the Mesh
      mesh->draw(shader, task.instances, task.subMesh);
    }
  }
}

void GLGraphicSystem::updateResources()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->update();
  for (map<string, GLFrame*>::iterator itr = mFrames.begin(); itr != mFrames.end(); ++itr)
    itr->second->update();
  for (map<string, GLShader*>::iterator itr = mShaders.begin(); itr != mShaders.end(); ++itr)
    itr->second->update();
  for (map<string, GLMesh*>::iterator itr = mMeshes.begin(); itr != mMeshes.end(); ++itr)
    itr->second->update();
  for (map<string, GLTexture*>::iterator itr = mTextures.begin(); itr != mTextures.end(); ++itr)
    itr->second->update();
}

void GLGraphicSystem::updateUniforms()
{
  for (list<GLUniformMap*>::iterator itr = mGLUniforms.begin(); itr != mGLUniforms.end();)
  {
    if ((*itr)->inUse())
    {
      (*itr)->update();
      ++itr;
    }
    else
    {
      delete *itr;
      itr = mGLUniforms.erase(itr);
    }
  }
}
