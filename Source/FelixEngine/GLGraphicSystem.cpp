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


GLGraphicSystem::GLGraphicSystem(): mContext(0), mMainWindow(0)
{
  mSDLInitFlags |= SDL_INIT_VIDEO;
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
  
  #if !__IPHONEOS__
  if (mMajorVersion > 2)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  #endif
  
  for (map<std::string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
  {
    success &= itr->second->init();
    if (!mMainWindow && success)
      mMainWindow = itr->second;
  }
  
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
  updateResources();
  updateUniforms();
  processTasks();
}


InternalUniformMap* GLGraphicSystem::getInternalUniformMap(const UniformMap *map)
{
  GLUniformMap *internalMap = new GLUniformMap(map);
  mGLUniforms.push_back(internalMap);
  return internalMap;
}

SDL_Window* GLGraphicSystem::getMainSDLWindow()
{
  return mMainWindow ? mMainWindow->getSDLWindow() : nullptr;
}

void GLGraphicSystem::processTasks()
{
  mPassesMutex.lock();
  int flags = getStereoFlags();
  if (mPasses.size())
  {
    if (flags & STEREO_MONO)
      processPass(mPasses.at(0), nullptr, STEREO_MONO);
    if (flags & STEREO_LEFT)
      processPass(mPasses.at(0), nullptr, STEREO_LEFT);
    if (flags & STEREO_RIGHT)
      processPass(mPasses.at(0), nullptr, STEREO_RIGHT);
  }
  mPassesMutex.unlock();
  
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->swapBuffers();
}

void GLGraphicSystem::processPass(const Pass &pass, const GraphicTask *view, int stereo)
{
  for (Pass::const_iterator itr = pass.begin(); itr != pass.end(); ++itr)
    processTask(&(*itr), view, stereo);
}

void GLGraphicSystem::processTask(const GraphicTask *task, const GraphicTask *view, int stereo)
{
  if (task->stereo & stereo)
  {
    if (task->isViewTask())
    {
      const GLFrame *frame = static_cast<const GLFrame*>(task->frame);
      frame->use(stereo);
      if (task->isClearTask())
        frame->clear(task->clearState);
      
      if (task->pass < (int)mPasses.size())
      {
        const Pass &pass = mPasses.at(task->pass);
        if (pass.size())
          processPass(pass, task, stereo);
      }
    }
    else if (task->isDrawTask())
    {
      const GLFrame  *frame  = static_cast<const GLFrame*>(view ? view->frame : task->frame);
      const GLShader *shader = static_cast<const GLShader*>(task->shader);
      const GLMesh   *mesh   = static_cast<const GLMesh*>(task->mesh);
      
      if (frame && frame->loaded() && shader->loaded() && mesh->loaded())
      {
        // Set the state for the Frame
        frame->use(stereo);
        if (task->isClearTask())
          frame->clear(task->clearState);
        frame->setDepthState(task->depthState);
        frame->setBlendState(task->blendState);
        
        // Set the state for the Shader
        shader->use();
        if (view && view->localUniforms)
          static_cast<const GLUniformMap*>(view->localUniforms)->applyToShader(shader);
        if (task->materialUniforms)
          static_cast<const GLUniformMap*>(task->materialUniforms)->applyToShader(shader);
        if (task->localUniforms)
          static_cast<const GLUniformMap*>(task->localUniforms)->applyToShader(shader);
        shader->applyTextureMap(task->textureMap);
        
        // Draw the Mesh
        mesh->draw(shader, task->instances, task->subMesh);
      }
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

int GLGraphicSystem::getStereoFlags() const
{
  int flags = 0;
  for (map<string, GLWindow*>::const_iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    flags |= itr->second->getStereoFlags();
  return flags;
}
