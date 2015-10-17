//
//  GLFrame.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLFrame.h"
#include "GLTexture.h"
#include "GLGraphicSystem.h"
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


GLFrame::GLFrame(GLGraphicSystem *system): mGLSystem(system), mFrameBufferId(0), mGLWindow(0)
{
}

GLFrame::~GLFrame()
{
  clearGLBuffers();
}

bool GLFrame::load()
{
  bool success = false;
  if (!isLoaded())
  {
    GLint curFrameBuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFrameBuffer);
    
    glGenFramebuffers(1, &mFrameBufferId);
    if (mFrameBufferId == 0)
      cerr << "Error Generating OpenGL Framebuffer" << endl;
    else
    {
      glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
      success = loadGLBuffers();
      glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
    }
  }
  return success;
}

void GLFrame::clearGLBuffers()
{
  
}
bool GLFrame::loadGLBuffers()
{
  bool success = true;
  
  int colorIndex = 0;
  for (list<Buffer>::const_iterator itr = mBuffers.begin(); itr != mBuffers.end(); ++itr)
  {
    if (itr->format == COLOR_RGBA)
    {
      mColorBuffers.push_back(GLBuffer());
      mColorBuffers.back().setIndex(colorIndex++);
      mColorBuffers.back().setFormat(itr->format);
      setTexture(mColorBuffers.back(), itr->name);
      success &= mColorBuffers.back().load(mSize);
    }
    else if (itr->format == COLOR_DEPTH32F)
    {
      mDepthBuffer.setFormat(itr->format);
      setTexture(mDepthBuffer, itr->name);
      success &= mDepthBuffer.load(mSize);
    }
    else if (itr->format == COLOR_STENCIL8)
    {
      mStencilBuffer.setFormat(itr->format);
      success &= mStencilBuffer.load(mSize);
    }
    
    if (mColorBuffers.size() != 0)
    {
      vector<GLenum> drawBuffers;
      for (int i = 0; i < (int)mColorBuffers.size(); ++i)
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0+i);
      glDrawBuffers((int)mColorBuffers.size(), &drawBuffers[0]);
    }
  }
  return success;
}

void GLFrame::setTexture(GLBuffer &buffer, const string &name)
{
  if (name != "")
  {
    GLTexture *texture = static_cast<GLTexture*>(mGLSystem->getTexture(name));
    buffer.setTexture(texture);
  }
}






bool GLFrame::GLBuffer::load(const ivec2 &size)
{
  bool success = false;
  if (mFormat != COLOR_NONE && size.w > 0 && size.h > 0)
  {
    if (mTexture != NULL)
      success = loadTextureBuffer(size);
    else
      success = loadRenderBuffer(size);
  }
  return success;
}

void GLFrame::GLBuffer::unload()
{
  // TODO: implement this method
}

bool GLFrame::GLBuffer::loadRenderBuffer(const ivec2 &size)
{
  bool success = false;
  GLint curRenderBuff;
  glGetIntegerv(GL_RENDERBUFFER_BINDING, &curRenderBuff);
  
  glGenRenderbuffers(1, &mBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, mBufferId);
  
  // TODO: support the other formats.
  if (mFormat == COLOR_RGBA)
  {
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.w, size.h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+mIndex, GL_RENDERBUFFER, mBufferId);
    success = true;
  }
  else if (mFormat == COLOR_DEPTH32F)
  {
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, size.w, size.h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mBufferId);
    success = true;
  }
  
  glBindRenderbuffer(GL_RENDERBUFFER, curRenderBuff);
  return success;
}

bool GLFrame::GLBuffer::loadTextureBuffer(const ivec2 &size)
{
  bool success = false;
  mBufferId = mTexture->useWithBuffer(size);
  glBindTexture(GL_TEXTURE_2D, mBufferId);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  if (!size.isPowerOfTwo())
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  
  // TODO: support the other formats.
  if (mFormat == COLOR_RGBA)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+mIndex, GL_TEXTURE_2D, mBufferId, 0);
    success = true;
  }
  else if (mFormat == COLOR_DEPTH32F)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, size.w, size.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mBufferId, 0);
    success = true;
  }
  
  glBindTexture(GL_TEXTURE_2D, 0);
  return success;
}



