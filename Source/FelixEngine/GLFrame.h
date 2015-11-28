//
//  GLFrame.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLFrame_h
#define GLFrame_h

#include "GLGraphicSystem.h"
#include "GLWindow.h"
#include "GLTexture.h"


namespace fx
{
  class GLWindow;
  
  /**
   *
   */
  class GLFrame: public Frame
  {
  public:
    GLFrame(GLGraphicSystem *system): mGLSystem(system), mFrameBufferId(0), mGLWindow(0) {}
    virtual ~GLFrame() {clearGLBuffers();}
    
    void update()
    {
      if (loading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
      else if (loaded())
      {
        if (mGLWindow)
          resize(mGLWindow->size());
        else if (mRefFrame)
          resize(mScale * mRefFrame->size());
      }
    }
    
    void setToWindow(GLWindow *window)
    {
      mGLWindow = window;
      mFrameBufferId = 0;
      setLoaded();
    }
    
    void use(int stereo) const
    {
      ivec2 size = mSize;
      if (mGLWindow)
        mGLWindow->setActive(stereo);
      else
      {
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
        glViewport(0, 0, size.w, size.h);
      }
    }
    
    void clear(const ClearState &clearState) const
    {
      GLbitfield clearFlags = 0;
      // TODO: update this to clear individual attachments.
      if (clearState.flags & CLEAR_COLOR)
      {
        RGBAf color = clearState.colors[0];
        glClearColor(color.red, color.green, color.blue, color.alpha);
        clearFlags = GL_COLOR_BUFFER_BIT;
      }
      if (clearState.flags & CLEAR_DEPTH)
      {
        glDepthMask(GL_TRUE);
        glClearDepthf(clearState.depth);
        clearFlags |= GL_DEPTH_BUFFER_BIT;
      }
      if (clearState.flags & CLEAR_STENCEIL)
      {
        glClearStencil(clearState.stenceil);
        clearFlags |= GL_STENCIL_BUFFER_BIT;
      }
      if (clearFlags)
        glClear(clearFlags);
    }
    
    void setDepthState(const DepthState &state) const
    {
      if (state.isTestingEnabled())
      {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(getGLDepthFunction(state.getTestFunction()));
      }
      else
        glDisable(GL_DEPTH_TEST);
      
      glDepthMask(state.isWritingEnabled() ? GL_TRUE : GL_FALSE);
      
      // Put in setCullFaceState method
      //glEnable(GL_CULL_FACE);
      //glCullFace(GL_BACK);
    }
    
    void setBlendState(const BlendState &state) const
    {
      if (state.isBlendingEnabled())
      {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
      else
        glDisable(GL_BLEND);
    }
    
  private:
    class GLBuffer
    {
    public:
      GLBuffer(): mIndex(0), mBufferId(0), mFormat(COLOR_NONE), mTexture(0) {}
      
      void setIndex(GLint index) {mIndex = index;}
      void setFormat(COLOR_TYPE format) {mFormat = format;}
      void setTexture(GLTexture *texture)
      {
        if (mTexture != texture)
        {
          if (mTexture)
            mTexture->release();
          if (texture)
            texture->retain();
          mTexture = texture;
        }
      }
      
      bool load(const ivec2 &size)
      {
        bool success = false;
        if (mFormat != COLOR_NONE && size.w > 0 && size.h > 0)
        {
          if (mTexture)
            success = loadTextureBuffer(size);
          else
            success = loadRenderBuffer(size);
        }
        return success;
      }
      
      void detach()
      {
        if (mTexture)
          detachTextureBuffer();
        else
          detachRenderBuffer();
      }
      
      void clear()
      {
        if (mBufferId)
        {
          if (!mTexture)
            glDeleteRenderbuffers(1, &mBufferId);
          else
          {
            mTexture->clearBuffer();
            glDeleteTextures(1, &mBufferId);
          }
          mBufferId = 0;
        }
      }
      
    private:
      bool loadRenderBuffer(const ivec2 &size)
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
        else if (mFormat == COLOR_DEPTH24)
        {
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.w, size.h);
          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mBufferId);
          success = true;
        }
        glBindRenderbuffer(GL_RENDERBUFFER, curRenderBuff);
        return success;
      }
      void detachRenderBuffer()
      {
        if (mBufferId > 0)
        {
          GLint curRenderBuff;
          glGetIntegerv(GL_RENDERBUFFER_BINDING, &curRenderBuff);
          glBindRenderbuffer(GL_RENDERBUFFER, mBufferId);
          
          // Detach the Render Buffer
          glBindRenderbuffer(GL_RENDERBUFFER, mBufferId);
          if (mFormat == COLOR_RGBA)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+mIndex, GL_RENDERBUFFER, 0);
          else
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
          glBindRenderbuffer(GL_RENDERBUFFER, curRenderBuff);
        }
      }
      
      bool loadTextureBuffer(const ivec2 &size)
      {
        bool success = false;
        
        glGenTextures(1, &mBufferId);
        mTexture->setBuffer(mBufferId, size);
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
        else if (mFormat == COLOR_DEPTH24)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.w, size.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mBufferId, 0);
          success = true;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return success;
      }
      void detachTextureBuffer()
      {
        if (mBufferId > 0)
        {
          // Detach the Frame Buffer Texture
          glBindTexture(GL_TEXTURE_2D, mBufferId);
          if (mFormat == COLOR_RGBA)
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
          else
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
          glBindTexture(GL_TEXTURE_2D, 0);
        }
      }
      
      GLuint mIndex, mBufferId;
      COLOR_TYPE mFormat;
      GLTexture *mTexture;
    };
    
  private:
    bool load()
    {
      bool success = false;
      if (!loaded())
      {
        GLint curFrameBuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFrameBuffer);
        
        if (mRefFrame)
          mSize = mRefFrame->size() * mScale;
        
        glGenFramebuffers(1, &mFrameBufferId);
        if (mFrameBufferId == 0)
          std::cerr << "Error Generating OpenGL Framebuffer" << std::endl;
        else
        {
          glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
          success = loadGLBuffers();
          glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
        }
      }
      return success;
    }
    
    void unload()
    {
      if (mFrameBufferId > 0)
      {
        GLint curFrameBuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFrameBuffer);
        
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
        detachGLBuffers();
        glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
        
        glDeleteFramebuffers(1, &mFrameBufferId);
        mFrameBufferId = 0;
        clearGLBuffers();
      }
      setUnloaded();
    }
    
    bool resize(const ivec2 &size)
    {
      bool success = true;
      if (mSize != size)
      {
        std::cout << "Resize to " << size << std::endl;
        unload();
        
        mSize = size;
        success = load();
        
        if (success)
          setLoaded();
      }
      return success;
    }
    
    bool loadGLBuffers()
    {
      bool success = true;
      
      int colorIndex = 0;
      for (std::list<Buffer>::const_iterator itr = mBuffers.begin(); itr != mBuffers.end(); ++itr)
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
          COLOR_TYPE format = mGLSystem->majorVersion() == 2 ? COLOR_DEPTH24 : COLOR_DEPTH32F;
          mDepthBuffer.setFormat(format);
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
          std::vector<GLenum> drawBuffers;
          for (int i = 0; i < (int)mColorBuffers.size(); ++i)
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0+i);
          glDrawBuffers((int)mColorBuffers.size(), &drawBuffers[0]);
        }
      }
      return success;
    }
    void detachGLBuffers()
    {
      for (std::list<GLBuffer>::iterator itr = mColorBuffers.begin(); itr != mColorBuffers.end(); ++itr)
        itr->detach();
      mDepthBuffer.detach();
    }
    void clearGLBuffers()
    {
      for (std::list<GLBuffer>::iterator itr = mColorBuffers.begin(); itr != mColorBuffers.end(); ++itr)
        itr->clear();
      mColorBuffers.clear();
      mDepthBuffer.clear();
    }
    
    void setTexture(GLBuffer &buffer, const std::string &name)
    {
      if (name != "")
      {
        GLTexture *texture = static_cast<GLTexture*>(mGLSystem->getTexture(name));
        buffer.setTexture(texture);
      }
    }
    
    GLenum getGLDepthFunction(int function) const
    {
      switch (function)
      {
        case DEPTH_TEST_LESS:       return GL_LESS;
        case DEPTH_TEST_GREATER:    return GL_GREATER;
        case DEPTH_TEST_EQUAL:      return GL_EQUAL;
        case DEPTH_TEST_LESS_EQ:    return GL_LEQUAL;
        case DEPTH_TEST_GREATER_EQ: return GL_GEQUAL;
        case DEPTH_TEST_NEVER:      return GL_NEVER;
        default:                    return GL_ALWAYS;
      }
    }
    
  private:
    GLuint mFrameBufferId;
    GLGraphicSystem *mGLSystem;
    GLWindow *mGLWindow;
    
    std::list<GLBuffer> mColorBuffers;
    GLBuffer mDepthBuffer;
    GLBuffer mStencilBuffer;
  };
}

#endif /* GLFrame_h */
