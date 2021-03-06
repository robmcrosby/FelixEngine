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
  for (auto &window : mWindows)
    delete window.second;
}

Window* GLGraphicSystem::getMainWindow()
{
  return getWindow(MAIN_WINDOW);
}

Window* GLGraphicSystem::getWindow(const std::string &name)
{
  if (!mWindows.count(name))
  {
    GLWindow *window = new GLWindow(this);
    mWindows[name] = window;
    
    GLFrame *frame = static_cast<GLFrame*>(getFrame(name));
    frame->setToWindow(window);
    window->setFrame(frame);
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
    success &= setShaderFunctions(node->subNode("Shaders"));
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
  
  for (auto &window : mWindows)
  {
    success &= window.second->init();
    if (!mMainWindow && success)
      mMainWindow = window.second;
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
    for (const auto &subNode : *node)
    {
      string name = subNode->attribute("name");
      if (name != "")
        mShaderFunctions[name] = subNode->contents();
    }
  }
  return success;
}

void GLGraphicSystem::render()
{
  updateTasks();
  
  // Update the Windows
  for (auto &window : mWindows)
    window.second->update();
  
  // Process the Graphic Tasks
  processPass(mPreTasks, nullptr);
  if (mTaskPasses.size())
      processPass(mTaskPasses.at(0), nullptr);
  processPass(mPostTasks, nullptr);
  
  // Swap the Window Buffers
  for (auto &window : mWindows)
    window.second->swapBuffers();
}

SDL_Window* GLGraphicSystem::getMainSDLWindow()
{
  return mMainWindow ? mMainWindow->getSDLWindow() : nullptr;
}

void GLGraphicSystem::processPass(const TaskPass &pass, const GraphicTask *view)
{
  for (const GraphicTask &task : pass)
    processTask(&task, view);
}

void GLGraphicSystem::processTask(const GraphicTask *task, const GraphicTask *view)
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
      processViewTask(task);
      break;
    case GRAPHIC_TASK_DRAW:
      processDrawTask(task, view);
    case GRAPHIC_TASK_COMPUTE:
    case GRAPHIC_TASK_EMPTY:
      break;
  }
}

void GLGraphicSystem::processUploadTask(const GraphicTask *task)
{
  Buffer *bufferMap = task->bufferSlots[0];
  if (bufferMap != nullptr && !bufferMap->updated())
  {
    if (bufferMap->bufferType() == BUFFER_MESH && bufferMap->resource())
    {
      GLMesh *mesh = static_cast<GLMesh*>(bufferMap->resource());
      mesh->uploadBufferMap(*bufferMap);
    }
    else if (bufferMap->bufferType() == BUFFER_PROGRAM && bufferMap->resource())
    {
      GLShader *shader = static_cast<GLShader*>(bufferMap->resource());
      shader->uploadBufferMap(*bufferMap);
    }
    else if (bufferMap->bufferType() == BUFFER_TEXTURE && bufferMap->resource())
    {
      if (bufferMap->contains("data"))
      {
        GLTexture *texture = GetResource<GLTexture>(bufferMap);
        texture->uploadBufferMap(*bufferMap);
      }
    }
    else if (bufferMap->bufferType() == BUFFER_FRAME && bufferMap->resource())
    {
      GLFrame *frame = GetResource<GLFrame>(bufferMap);
      frame->uploadBufferMap(*bufferMap);
    }
    else if (bufferMap->bufferType() == BUFFER_UNIFORM)
    {
      GLUniforms *uniforms = GetResource<GLUniforms>(bufferMap);
      if (!uniforms || !mUniforms.count(uniforms))
      {
        uniforms = new GLUniforms();
        mUniforms.insert(uniforms);
        bufferMap->setResource(uniforms);
      }
      uniforms->upload(*bufferMap);
    }
    bufferMap->setUpdated();
  }
}

void GLGraphicSystem::processUnloadTask(const GraphicTask *task)
{
  // TODO: Implement this when needed.
}

void GLGraphicSystem::processDownloadTask(const GraphicTask *task)
{
  // TODO: Implement this when needed.
}

void GLGraphicSystem::processViewTask(const GraphicTask *task)
{
  const GLFrame *frame = GetResource<GLFrame>(task->bufferSlots[BUFFER_SLOT_FRAME]);
  if (frame)
  {
    frame->bind();
    frame->setViewport(task->drawState.viewport);
    frame->clear(task->drawState.clearState);
  }
  
  if (task->pass < mTaskPasses.size() && task->pass > 0)
    processPass(mTaskPasses.at(task->pass), task);
}

void GLGraphicSystem::processDrawTask(const GraphicTask *task, const GraphicTask *view)
{
  const GLShader *shader = GetResource<GLShader>(task->bufferSlots[BUFFER_SLOT_SHADER]);
  const GLMesh   *mesh   = GetResource<GLMesh>(task->bufferSlots[BUFFER_SLOT_MESH]);
  
  Viewport viewport;
  GLUniforms *uniforms[5];
  for (int i = 0; i < 4; ++i)
    uniforms[i] = GetResource<GLUniforms>(task->bufferSlots[i]);
  
  // Get the Render Targets and View Uniforms.
  const GLFrame  *frame = nullptr;
  if (view)
  {
    frame = GetResource<GLFrame>(view->bufferSlots[BUFFER_SLOT_FRAME]);
    viewport = view->drawState.viewport;
    uniforms[4] = GetResource<GLUniforms>(view->bufferSlots[0]);
  }
  else
  {
    frame = GetResource<GLFrame>(task->bufferSlots[BUFFER_SLOT_FRAME]);
    viewport = task->drawState.viewport;
    uniforms[4] = nullptr;
  }
  
  // If no Render Targets are specified, use the main window.
  if (!frame)
    frame = static_cast<GLFrame*>(getFrame("MainWindow"));
  
  if (frame && frame->loaded() && shader && shader->loaded() && mesh && mesh->loaded())
  {
    setTriangleCullMode(task->drawState.cullMode);
    
    // Set the state for the Shader
    shader->bind();
    
    // Set the state for the Frame
    frame->bind();
    frame->setViewport(viewport);
    frame->clear(task->drawState.clearState);
    frame->setDepthState(task->drawState.depthState);
    frame->setBlendState(task->drawState.blendState);
    
    // Bind the Mesh to the Shader
    mesh->bind(shader);
    
    // Apply the Uniforms
    for (int i = 0; i < 5; ++i)
    {
      if (uniforms[i])
        uniforms[i]->applyToShader(shader);
    }
    
    // Set the Textures and Draw the Mesh
    if (bindTextures(task->bufferSlots[BUFFER_SLOT_TEXTURES]))
      mesh->draw(task->drawState.instances, task->drawState.submesh);
  }
}

bool GLGraphicSystem::bindTextures(Buffer *textures)
{
  bool success = true;
  if (textures)
  {
    int index = 0;
    for (Buffer &buffer : *textures)
    {
      GLTexture *texture = GetResource<GLTexture>(&buffer);
      if (!texture || !texture->loaded())
        success = false;
      else
        texture->use(index, Sampler(buffer.flags()));
    }
  }
  return success;
}

bool GLGraphicSystem::addWindows(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    success = true;
    for (auto const &subNode : *node)
      success &= addWindow(subNode);
  }
  return success;
}

bool GLGraphicSystem::addWindow(const XMLTree::Node *node)
{
  bool success = false;
  if (node)
  {
    string name = node->hasSubNode("name") ? node->attribute("name") : MAIN_WINDOW;
    GLWindow *window = static_cast<GLWindow*>(getWindow(name));
    success = window && window->setToXml(*node);
  }
  return success;
}


void GLGraphicSystem::setTriangleCullMode(CULL_TRI_MODE mode)
{
  static CULL_TRI_MODE curMode = CULL_TRI_NONE;
  if (curMode != mode)
  {
    curMode = mode;
    if (mode != CULL_TRI_NONE)
    {
      glEnable(GL_CULL_FACE);
      glCullFace((mode == CULL_TRI_BACK) ? GL_BACK : GL_FRONT);
    }
    else
      glDisable(GL_CULL_FACE);
  }
}
