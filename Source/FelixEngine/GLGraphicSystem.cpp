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
#include "GLUniforms.h"


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
  updateTasks();
  int flags = getStereoFlags();
  
  // Update the Windows
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->update();
  
  // Process the Graphic Tasks
  processPass(mPreTasks, nullptr, 0);
  if (mTaskPasses.size())
  {
    if (flags & STEREO_MONO)
      processPass(mTaskPasses.at(0), nullptr, STEREO_MONO);
    if (flags & STEREO_LEFT)
      processPass(mTaskPasses.at(0), nullptr, STEREO_LEFT);
    if (flags & STEREO_RIGHT)
      processPass(mTaskPasses.at(0), nullptr, STEREO_RIGHT);
  }
  processPass(mPostTasks, nullptr, 0);
  
  // Swap the Window Buffers
  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    itr->second->swapBuffers();
}

SDL_Window* GLGraphicSystem::getMainSDLWindow()
{
  return mMainWindow ? mMainWindow->getSDLWindow() : nullptr;
}
//
//void GLGraphicSystem::processTasks()
//{
////  mPassesMutex.lock();
////  int flags = getStereoFlags();
////  if (mPasses.size())
////  {
////    if (flags & STEREO_MONO)
////      processPass(mPasses.at(0), nullptr, STEREO_MONO);
////    if (flags & STEREO_LEFT)
////      processPass(mPasses.at(0), nullptr, STEREO_LEFT);
////    if (flags & STEREO_RIGHT)
////      processPass(mPasses.at(0), nullptr, STEREO_RIGHT);
////  }
////  mPassesMutex.unlock();
////  
////  for (map<string, GLWindow*>::iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
////    itr->second->swapBuffers();
//}

void GLGraphicSystem::processPass(const TaskPass &pass, const GraphicTask *view, int stereo)
{
  for (TaskPass::const_iterator itr = pass.begin(); itr != pass.end(); ++itr)
    processTask(&(*itr), view, stereo);
}

void GLGraphicSystem::processTask(const GraphicTask *task, const GraphicTask *view, int stereo)
{
  switch (task->type)
  {
    case GRAPHIC_TASK_UPLOAD:
      processUploadTask(task);
      break;
    case GRAPHIC_TASK_UNLOAD:
      processUnloadTask(task);
      break;
    case GRAPHIC_TASK_DOWNLOAD:
      processDownloadTask(task);
      break;
    case GRAPHIC_TASK_PASS:
      processViewTask(task, stereo);
      break;
    case GRAPHIC_TASK_DRAW:
      processDrawTask(task, view, stereo);
    case GRAPHIC_TASK_COMPUTE:
    case GRAPHIC_TASK_EMPTY:
      break;
  }
}

void GLGraphicSystem::processUploadTask(const GraphicTask *task)
{
  BufferMap *bufferMap = task->bufferSlots[0];
  if (bufferMap != nullptr)
  {
    if (bufferMap->type() == BUFFER_MAP_MESH)
    {
      bufferMap->setResource(getMesh(bufferMap->name()));
      GLMesh *mesh = static_cast<GLMesh*>(bufferMap->resource());
      mesh->uploadBufferMap(*bufferMap);
    }
    else if (bufferMap->type() == BUFFER_MAP_SHADER)
    {
      bufferMap->setResource(getShader(bufferMap->name()));
      GLShader *shader = static_cast<GLShader*>(bufferMap->resource());
      shader->uploadBufferMap(*bufferMap);
    }
    else if (bufferMap->type() == BUFFER_MAP_UNIFORMS)
    {
      GLUniforms *uniforms = GetResource<GLUniforms>(bufferMap);
      if (!uniforms || !mUniforms.count(uniforms))
      {
        uniforms = new GLUniforms();
        mUniforms.insert(uniforms);
        bufferMap->setResource(uniforms);
      }
      uniforms->uploadBufferMap(*bufferMap);
    }
  }
}

void GLGraphicSystem::processUnloadTask(const GraphicTask *task)
{
  
}

void GLGraphicSystem::processDownloadTask(const GraphicTask *task)
{
  
}

void GLGraphicSystem::processViewTask(const GraphicTask *task, int stereo)
{
  if (task->pass < mTaskPasses.size())
    processPass(mTaskPasses.at(task->pass), task, stereo);
}

void GLGraphicSystem::processDrawTask(const GraphicTask *task, const GraphicTask *view, int stereo)
{
  if (task->drawState.stereo & stereo)
  {
    const GLFrame  *frame  = mFrames.begin()->second;
    const GLShader *shader = GetResource<GLShader>(task->bufferSlots[BUFFER_SLOT_SHADER]);
    const GLMesh   *mesh   = GetResource<GLMesh>(task->bufferSlots[BUFFER_SLOT_MESH]);
    
    const GLUniforms *localUniforms = GetResource<GLUniforms>(task->bufferSlots[BUFFER_SLOT_UNIFORMS]);
    
    if (shader && shader->loaded() && mesh && mesh->loaded())
    {
      setTriangleCullMode(task->drawState.cullMode);
      
      // Set the state for the Frame
      frame->use(stereo);
      frame->clear(task->drawState.clearState);
      frame->setDepthState(task->drawState.depthState);
      frame->setBlendState(task->drawState.blendState);
      
      // Set the state for the Shader
      shader->use();
      mesh->bind(shader);
      
      if (localUniforms)
        localUniforms->applyToShader(shader);
      
//      if (view && view->localUniforms)
//        static_cast<const GLUniformMap*>(view->localUniforms)->applyToShader(shader);
//      if (task->materialUniforms)
//        static_cast<const GLUniformMap*>(task->materialUniforms)->applyToShader(shader);
//      if (task->localUniforms)
//        static_cast<const GLUniformMap*>(task->localUniforms)->applyToShader(shader);
//      shader->applyTextureMap(task->textureMap);
      
      // Draw the Mesh
      mesh->draw(task->drawState.instances, task->drawState.submesh);
    }
  }
}





//void GLGraphicSystem::processTask(const GraphicTask *task, const GraphicTask *view, int stereo)
//{
////  if (task->stereo & stereo)
////  {
////    if (task->isViewTask())
////    {
////      const GLFrame *frame = static_cast<const GLFrame*>(task->frame);
////      frame->use(stereo);
////      if (task->isClearTask())
////        frame->clear(task->clearState);
////      
////      if (task->pass < (int)mPasses.size())
////      {
////        const Pass &pass = mPasses.at(task->pass);
////        if (pass.size())
////          processPass(pass, task, stereo);
////      }
////    }
////    else if (task->isDrawTask())
////    {
////      const GLFrame  *frame  = static_cast<const GLFrame*>(view ? view->frame : task->frame);
////      const GLShader *shader = static_cast<const GLShader*>(task->shader);
////      const GLMesh   *mesh   = static_cast<const GLMesh*>(task->mesh);
////      
////      if (frame && frame->loaded() && shader->loaded() && mesh->loaded())
////      {
////        setTriangleCullMode(task->cullMode);
////        
////        // Set the state for the Frame
////        frame->use(stereo);
////        if (task->isClearTask())
////          frame->clear(task->clearState);
////        frame->setDepthState(task->depthState);
////        frame->setBlendState(task->blendState);
////        
////        // Set the state for the Shader
////        shader->use();
////        mesh->bind(shader);
////        
////        if (view && view->localUniforms)
////          static_cast<const GLUniformMap*>(view->localUniforms)->applyToShader(shader);
////        if (task->materialUniforms)
////          static_cast<const GLUniformMap*>(task->materialUniforms)->applyToShader(shader);
////        if (task->localUniforms)
////          static_cast<const GLUniformMap*>(task->localUniforms)->applyToShader(shader);
////        shader->applyTextureMap(task->textureMap);
////        
////        // Draw the Mesh
////        mesh->draw(task->instances, task->subMesh);
////      }
////    }
////  }
//}

void GLGraphicSystem::setTriangleCullMode(fx::CULL_MODE mode)
{
  static CULL_MODE curMode = CULL_NONE;
  if (curMode != mode)
  {
    curMode = mode;
    if (mode != CULL_NONE)
    {
      glEnable(GL_CULL_FACE);
      glCullFace((mode == CULL_BACK) ? GL_BACK : GL_FRONT);
    }
    else
      glDisable(GL_CULL_FACE);
  }
}

int GLGraphicSystem::getStereoFlags() const
{
  int flags = 0;
  for (map<string, GLWindow*>::const_iterator itr = mWindows.begin(); itr != mWindows.end(); ++itr)
    flags |= itr->second->getStereoFlags();
  return flags;
}
