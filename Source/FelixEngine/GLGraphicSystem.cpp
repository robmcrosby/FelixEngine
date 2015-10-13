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


GLGraphicSystem::GLGraphicSystem(): mContext(0), mCheckForUnloaded(0)
{
  mInitFlags |= SDL_INIT_VIDEO;
}

GLGraphicSystem::~GLGraphicSystem()
{
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    delete itr->second;
}

Window* GLGraphicSystem::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
    mWindows[name] = new GLWindow(this);
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
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
}

bool GLGraphicSystem::setToXml(const XMLTree::Node *node)
{
  if (node)
  {
    setVersion(node->subNode("Version"));
    return addWindows(node->subNode("Windows"));
  }
  cerr << "Error: GLGraphicSystem passed a NULL node." << endl;
  return false;
}

bool GLGraphicSystem::init()
{
  bool success = true;
  for (map<std::string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    success &= itr->second->load();
  return success;
}

bool GLGraphicSystem::setVersion(const XMLTree::Node *node)
{
  if (node && node->hasAttribute("major") && node->hasAttribute("minor"))
  {
    setVersion(node->attributeAsInt("major"), node->attributeAsInt("minor"));
    return true;
  }
  
  cerr << "Warning: Unable to determine OpenGL Version from settings, using 2.1 for now." << endl;
  setVersion(2, 1);
  return false;
}

void GLGraphicSystem::update()
{
  loadTaskSlots();
  
  
//  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
//  {
//    itr->second->setActive();
//    
//    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
//    
//    itr->second->swapBuffers();
//  }
  checkForUnloaded();
}

InternalUniformMap* GLGraphicSystem::createUniformMap()
{
  return new GLUniformMap();
}

void GLGraphicSystem::processTasks()
{
  clearTaskSlots();
  loadTaskSlots();
  processTaskSlot(mTaskSlots.at(0));
}

void GLGraphicSystem::processTaskSlot(const TaskSlot &slot)
{
  for (TaskSlot::const_iterator itr = slot.begin(); itr != slot.end(); ++itr)
    processTask(*itr);
}

void GLGraphicSystem::processTask(const GraphicTask &task)
{
  if (task.isViewTask() && task.mViewIndex < (int)mTaskSlots.size())
  {
    const TaskSlot &slot = mTaskSlots.at(task.mViewIndex);
    if (slot.size())
    {
      const GLFrame *frame = static_cast<const GLFrame*>(task.mFrame);
      frame->use();
      if (task.isClearTask())
        frame->clear(task.mClearState);
      processTaskSlot(slot);
    }
  }
  else if (task.isDrawTask())
  {
    const GLFrame  *frame  = static_cast<const GLFrame*>(task.mFrame);
    const GLShader *shader = static_cast<const GLShader*>(task.mShader);
    const GLMesh   *mesh   = static_cast<const GLMesh*>(task.mMesh);
    
    // Set the state for the Frame
    frame->use();
    if (task.isClearTask())
      frame->clear(task.mClearState);
    frame->setDepthState(task.mDepthState);
    frame->setBlendState(task.mBlendState);
    
    // Set the state for the Shader
    shader->use();
    if (task.mViewUniforms)
      static_cast<const GLUniformMap*>(task.mViewUniforms)->applyToShader(shader);
    if (task.mMaterialUniforms)
      static_cast<const GLUniformMap*>(task.mMaterialUniforms)->applyToShader(shader);
    if (task.mLocalUniforms)
      static_cast<const GLUniformMap*>(task.mLocalUniforms)->applyToShader(shader);
    
    // Draw the Mesh
    mesh->draw(shader, task.mInstances, task.mSubMesh);
  }
}

void GLGraphicSystem::checkForUnloaded()
{
  if (mCheckForUnloaded)
  {
    cout << "Check for Unloaded" << endl;
    for (std::map<std::string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
      itr->second->load();
    for (std::map<std::string, GLFrame*>::iterator itr = mFrames.begin(); itr != mFrames.end(); ++itr)
      itr->second->load();
    for (std::map<std::string, GLShader*>::iterator itr = mShaders.begin(); itr != mShaders.end(); ++itr)
      itr->second->load();
    for (std::map<std::string, GLMesh*>::iterator itr = mMeshes.begin(); itr != mMeshes.end(); ++itr)
      itr->second->load();
    for (std::map<std::string, GLTexture*>::iterator itr = mTextures.begin(); itr != mTextures.end(); ++itr)
      itr->second->load();
    mCheckForUnloaded = false;
  }
}
