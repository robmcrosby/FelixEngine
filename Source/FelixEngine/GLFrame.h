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


namespace fx
{
  class GLWindow;
  
  /**
   *
   */
  class GLFrame: public Frame
  {
  public:
    GLFrame(GLGraphicSystem *system);
    virtual ~GLFrame();
    
    void update()
    {
      if (isLoading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    
    void setToWindow(GLWindow *window)
    {
      mGLWindow = window;
      mFrameBufferId = 0;
      setLoaded();
    }
    
    void use() const
    {
      ivec2 size = mSize;
      if (mGLWindow)
      {
        size = mGLWindow->size();
        mGLWindow->setActive();
      }
      glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
      glViewport(0, 0, size.w, size.h);
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
        glClearDepth(clearState.depth);
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
      void setTexture(GLTexture *texture) {mTexture = texture;}
      
      bool load(const ivec2 &size);
      void unload();
      
    private:
      bool loadRenderBuffer(const ivec2 &size);
      bool loadTextureBuffer(const ivec2 &size);
      
      GLuint mIndex, mBufferId;
      COLOR_TYPE mFormat;
      GLTexture *mTexture;
    };
    
  private:
    bool load();
    void clearGLBuffers();
    bool loadGLBuffers();
    void setTexture(GLBuffer &buffer, const std::string &name);
    
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
