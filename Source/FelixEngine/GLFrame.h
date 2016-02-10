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
   * Encapsolates an OpenGL Frame Buffer Object (FBO) which controls the render targets in rendering.
   */
  class GLFrame: public Frame
  {
  public:
    /**
     * Constructor that initalizes the Frame.
     *
     * @param system Pointer to a GLGraphicSystem.
     */
    GLFrame(GLGraphicSystem *system): mGLSystem(system), mFrameBufferId(0), mGLWindow(0) {}
    
    /**
     * Destructor
     */
    virtual ~GLFrame() {unload();}
    
    /**
     * Updates the size if refrencing a window or another frame buffer.
     */
    void update()
    {
      if (loaded())
      {
        if (mGLWindow)
          mSize = mGLWindow->frameSize();
        else if (mRefFrame)
          resize(mScale * mRefFrame->size());
      }
    }
    
    /**
     * Sets the Frame to refrence a Window.
     *
     * @param window Pointer to a GLWindow to refrence.
     */
    void setToWindow(GLWindow *window)
    {
      mGLWindow = window;
      mFrameBufferId = 0;
      setLoaded();
    }
    
    /**
     * Calls OpenGL to bind the FBO.
     *
     * @param stereo Flags to pass to window if used.
     */
    void bind(int stereo) const
    {
      ivec2 size = mSize;
      if (mGLWindow)
        mGLWindow->setActive(stereo);
      else
      {
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
        glViewport(0, 0, size.w, size.h);
        glDisable(GL_SCISSOR_TEST);
      }
    }
    
    /**
     * Clears the FBO with the settings from the given ClearState.
     *
     * @param state Refrence to a ClearState.
     */
    void clear(const ClearState &state) const
    {
      GLbitfield flags = 0;
      // TODO: update this to clear individual attachments.
      if (state.flags & CLEAR_COLOR)
      {
        RGBAf color = state.colors[0];
        glClearColor(color.red, color.green, color.blue, color.alpha);
        flags = GL_COLOR_BUFFER_BIT;
      }
      if (state.flags & CLEAR_DEPTH)
      {
        glDepthMask(GL_TRUE);
        glClearDepthf(state.depth);
        flags |= GL_DEPTH_BUFFER_BIT;
      }
      if (state.flags & CLEAR_STENCEIL)
      {
        glClearStencil(state.stenceil);
        flags |= GL_STENCIL_BUFFER_BIT;
      }
      if (flags)
        glClear(flags);
    }
    
    /**
     * Sets how the depth buffer would be used from the given DepthState.
     *
     * @param state Refrence to a DepthState.
     */
    void setDepthState(const DepthState &state) const
    {
      if (state.testingEnabled())
      {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GetGLDepthFunction(state.function()));
      }
      else
        glDisable(GL_DEPTH_TEST);
      glDepthMask(state.writingEnabled() ? GL_TRUE : GL_FALSE);
    }
    
    /**
     * Sets how blending would be handled from the given BlendState
     *
     * @param state Refrence to a BlendState.
     */
    void setBlendState(const BlendState &state) const
    {
      if (state.enabled())
      {
        glEnable(GL_BLEND);
        GLenum src = GetGLBlendInput(state.src());
        GLenum dst = GetGLBlendInput(state.dst());
        GLenum eq  = GetGLBlendEquation(state.equation());
        
        if (state.seperate())
        {
          GLenum srcAlpha = GetGLBlendInput(state.srcAlpha());
          GLenum dstAlpha = GetGLBlendInput(state.dstAlpha());
          GLenum eqAlpha  = GetGLBlendEquation(state.equationAlpha());
          
          glBlendEquationSeparate(eq, eqAlpha);
          glBlendFuncSeparate(src, dst, srcAlpha, dstAlpha);
        }
        else
        {
          glBlendEquation(eq);
          glBlendFunc(src, dst);
        }
        
        glBlendColor(state.color.red, state.color.green, state.color.blue, state.color.alpha);
      }
      else
        glDisable(GL_BLEND);
    }
    
  private:
    /**
     * Internal class that manages a single OpenGL Frame Buffer.
     */
    struct GLBuffer
    {
      /**
       * Constructor initalizes to an empty Frame Buffer.
       */
      GLBuffer(): index(0), bufferId(0), format(COLOR_NONE), texture(0) {}
      
      /**
       * Checks some conditions and creates either a texture or render buffer to the given size.
       *
       * @param size Refrence to a 2d intenger vector for the size of the buffer.
       * @return true if the buffer loaded correctly or false otherwise.
       */
      bool create(const ivec2 &size)
      {
        if (format != COLOR_NONE && size.w > 0 && size.h > 0)
          return texture ? createTextureBuffer(size) : createRenderBuffer(size);
        return false;
      }
      
      /**
       * Detaches either the texture or the render buffer from the FBO.
       */
      void detach()
      {
        if (texture)
          detachTextureBuffer();
        else
          detachRenderBuffer();
      }
      
      /**
       * Deletes either the texture or the render buffer.
       */
      void destroy()
      {
        if (bufferId)
        {
          if (!texture)
            glDeleteRenderbuffers(1, &bufferId);
          else
          {
            texture->clearBuffer();
            glDeleteTextures(1, &bufferId);
          }
          bufferId = 0;
        }
      }
      
      /**
       * Creates a Render Buffer to the given size.
       *
       * @param size Refrence to a 2d intenger vector for the size of the buffer.
       * @return true if successfully created or false otherwise.
       */
      bool createRenderBuffer(const ivec2 &size)
      {
        bool success = false;
        GLint curRenderBuff;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &curRenderBuff);
        
        glGenRenderbuffers(1, &bufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
        
        // TODO: support the other formats.
        if (format == COLOR_RGBA)
        {
          glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.w, size.h);
          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, GL_RENDERBUFFER, bufferId);
          success = true;
        }
        else if (format == COLOR_DEPTH32F)
        {
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, size.w, size.h);
          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufferId);
          success = true;
        }
        else if (format == COLOR_DEPTH24)
        {
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.w, size.h);
          glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufferId);
          success = true;
        }
        glBindRenderbuffer(GL_RENDERBUFFER, curRenderBuff);
        return success;
      }
      
      /**
       * Detaches this Render Buffer from the FBO.
       */
      void detachRenderBuffer()
      {
        if (bufferId > 0)
        {
          GLint curRenderBuff;
          glGetIntegerv(GL_RENDERBUFFER_BINDING, &curRenderBuff);
          glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
          
          // Detach the Render Buffer
          glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
          if (format == COLOR_RGBA)
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, GL_RENDERBUFFER, 0);
          else
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
          glBindRenderbuffer(GL_RENDERBUFFER, curRenderBuff);
        }
      }
      
      /**
       * Creates a Texture Buffer to the given size.
       *
       * @param size Refrence to a 2d intenger vector for the size of the buffer.
       * @return true if successfully created or false otherwise.
       */
      bool createTextureBuffer(const ivec2 &size)
      {
        bool success = false;
        
        glGenTextures(1, &bufferId);
        texture->setBuffer(bufferId, size);
        glBindTexture(GL_TEXTURE_2D, bufferId);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        if (!size.isPowerOfTwo())
        {
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        
        // TODO: support the other formats.
        if (format == COLOR_RGBA)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, GL_TEXTURE_2D, bufferId, 0);
          success = true;
        }
        else if (format == COLOR_DEPTH32F)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, size.w, size.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferId, 0);
          success = true;
        }
        else if (format == COLOR_DEPTH24)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size.w, size.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
          glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferId, 0);
          success = true;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return success;
      }
      
      /**
       * Detaches this Texture Buffer from the FBO.
       */
      void detachTextureBuffer()
      {
        if (bufferId > 0)
        {
          // Detach the Frame Buffer Texture
          glBindTexture(GL_TEXTURE_2D, bufferId);
          if (format == COLOR_RGBA)
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
          else
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
          glBindTexture(GL_TEXTURE_2D, 0);
        }
      }
      
      GLuint index;    /**< Index of the buffer */
      GLuint bufferId; /**< OpenGL buffer handle */
      
      COLOR_TYPE format;  /**< Format of the buffer */
      GLTexture *texture; /**< Pointer to a GLTexture if a Texture Buffer */
    };
    
    
  public:
    /**
     * Upload from a BufferMap
     *
     * @param bufferMap Refrence to a BufferMap that defines the Frame.
     */
    void uploadBufferMap(const BufferMap &bufferMap)
    {
      bool success = false;
      unload();
      
      GLint curFrameBuffer;
      glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFrameBuffer);
      
      // Create a new FrameBuffer
      glGenFramebuffers(1, &mFrameBufferId);
      if (mFrameBufferId == 0)
        std::cerr << "Error Generating OpenGL Framebuffer" << std::endl;
      else
      {
        // Determine the Size
        if (bufferMap.contains("size"))
          mSize = bufferMap["size"].ivec2Value();
        else if (bufferMap.contains("reference"))
        {
          setRefrenceFrame(bufferMap["reference"].stringValue());
          if (bufferMap.contains("scale"))
            mScale = bufferMap["scale"].vec2Value();
          else
            mScale = vec2(1.0f, 1.0f);
          
          if (mRefFrame)
            mSize = mRefFrame->size() * mScale;
          else
            mSize = ivec2(0, 0);
        }
        
        if (mSize > ivec2(0, 0))
        {
          success = true;
          glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
          // Add the Targets
          for (BufferMap::const_iterator itr = bufferMap.begin(); itr != bufferMap.end(); ++itr)
          {
            if (itr->bufferType() == BUFFER_TARGET)
              success &= addBuffer(*itr);
          }
          
          // Set the Color Attachments
          if (mColorBuffers.size() != 0)
          {
            std::vector<GLenum> drawBuffers;
            for (int i = 0; i < (int)mColorBuffers.size(); ++i)
              drawBuffers.push_back(GL_COLOR_ATTACHMENT0+i);
            glDrawBuffers((int)mColorBuffers.size(), &drawBuffers[0]);
          }
          
          glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
        }
      }
      setLoaded(success);
    }
    
    /**
     * Unloads the FBO
     */
    void unload()
    {
      if (mFrameBufferId > 0)
      {
        GLint curFrameBuffer;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFrameBuffer);
        
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
        detachBuffers();
        glBindFramebuffer(GL_FRAMEBUFFER, curFrameBuffer);
        
        glDeleteFramebuffers(1, &mFrameBufferId);
        mFrameBufferId = 0;
        destroyBuffers();
      }
      setUnloaded();
    }
    
    /**
     * Resizes all the buffers in the FBO to the given size.
     *
     * @param size Refrence to a 2d intenger vector for the new size.
     * @return true if successful or false otherwise.
     */
    bool resize(const ivec2 &size)
    {
      bool success = true;
      return success;
    }
    
  private:
    
    /**
     * Adds either a Texture or Render buffer defined by the given Buffer.
     *
     * @param buffer Refrence to a Buffer that defines the type of Buffer to add to the FBO.
     * @return true if successfully added or false otherwise.
     */
    bool addBuffer(const Buffer &buffer)
    {
      COLOR_TYPE format = (COLOR_TYPE)buffer.flags();
      if (format == COLOR_RGBA)
      {
        mColorBuffers.push_back(GLBuffer());
        mColorBuffers.back().index = (int)mColorBuffers.size()-1;
        mColorBuffers.back().format = format;
        setTextureToBuffer(buffer.name(), mColorBuffers.back());
        return mColorBuffers.back().create(mSize);
      }
      if (format == COLOR_DEPTH32F)
      {
        format = mGLSystem->majorVersion() == 2 ? COLOR_DEPTH24 : COLOR_DEPTH32F;
        mDepthBuffer.format = format;
        setTextureToBuffer(buffer.name(), mDepthBuffer);
        return mDepthBuffer.create(mSize);
      }
      if (format == COLOR_STENCIL8)
      {
        mStencilBuffer.format = format;
        return mStencilBuffer.create(mSize);
      }
      return false;
    }
    
    /**
     * Detaches all the buffers from the FBO.
     */
    void detachBuffers()
    {
      for (std::list<GLBuffer>::iterator itr = mColorBuffers.begin(); itr != mColorBuffers.end(); ++itr)
        itr->detach();
      mDepthBuffer.detach();
    }
    
    /**
     * Destroys all the buffers and clears out the Color Buffers.
     */
    void destroyBuffers()
    {
      for (std::list<GLBuffer>::iterator itr = mColorBuffers.begin(); itr != mColorBuffers.end(); ++itr)
        itr->destroy();
      mColorBuffers.clear();
      mDepthBuffer.destroy();
    }
    
    /**
     * Sets a Texture from the GLGraphicSystem with the given name to the given buffer.
     *
     * @param name String name of the texture.
     * @param buffer Refrence to a GLBuffer to assign the texture to.
     */
    void setTextureToBuffer(const std::string &name, GLBuffer &buffer)
    {
      if (name != "")
      {
        GLTexture *texture = static_cast<GLTexture*>(mGLSystem->getTexture(name));
        Resource::Replace(&buffer.texture, texture);
      }
    }
    
    /**
     * Gets an OpenGL enum for the given depth testing function.
     *
     * @param function Intenger value equal to a depth function in DEPTH_FLAGS.
     * @return OpenGL depth testing function enum.
     */
    static GLenum GetGLDepthFunction(int function)
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
    
    /**
     * Gets OpenGL flags for the given blend input flags.
     *
     * @param input Intenger value from combined BLEND_FLAGS.
     * @return OpenGL blend input enum.
     */
    static GLenum GetGLBlendInput(int input)
    {
      if (input == BLEND_INPUT_SRC_ALPHA_SATURATE)
        return GL_SRC_ALPHA_SATURATE;
      if (input & BLEND_INPUT_ONE_MINUS)
      {
        input &= ~BLEND_INPUT_ONE_MINUS;
        switch (input)
        {
          case BLEND_INPUT_SRC_COLOR:   return GL_ONE_MINUS_SRC_COLOR;
          case BLEND_INPUT_SRC_ALPHA:   return GL_ONE_MINUS_SRC_ALPHA;
          case BLEND_INPUT_DST_COLOR:   return GL_ONE_MINUS_DST_COLOR;
          case BLEND_INPUT_DST_ALPHA:   return GL_ONE_MINUS_DST_ALPHA;
          case BLEND_INPUT_CONST_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
          case BLEND_INPUT_CONST_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
        }
      }
      else
      {
        switch (input)
        {
          case BLEND_INPUT_SRC_COLOR:   return GL_SRC_COLOR;
          case BLEND_INPUT_SRC_ALPHA:   return GL_SRC_ALPHA;
          case BLEND_INPUT_DST_COLOR:   return GL_DST_COLOR;
          case BLEND_INPUT_DST_ALPHA:   return GL_DST_ALPHA;
          case BLEND_INPUT_CONST_COLOR: return GL_CONSTANT_COLOR;
          case BLEND_INPUT_CONST_ALPHA: return GL_CONSTANT_ALPHA;
          case BLEND_INPUT_ZERO:        return GL_ZERO;
        }
      }
      return GL_ONE;
    }
    
    /**
     * Gets an OpenGL enum for the given blend equation.
     *
     * @param eq Intenger value equal to a blend equation in BLEND_FLAGS.
     * @return OpenGL blend equation enum.
     */
    static GLenum GetGLBlendEquation(int eq)
    {
      // TODO: add the other blend functions.
      return GL_FUNC_ADD;
    }
    
  private:
    GLuint mFrameBufferId; /**< OpenGL handle for a FBO */
    GLWindow *mGLWindow;   /**< Pointer to a GLWindow if owned by a Window or Null otherwise. */
    
    std::list<GLBuffer> mColorBuffers; /**< List of Color Buffers */
    
    GLBuffer mDepthBuffer;   /**< Depth Buffer */
    GLBuffer mStencilBuffer; /**< Stencil Buffer */
    
    GLGraphicSystem *mGLSystem; /**< Pointer to the GLGraphicsSystem that owns this Frame */
  };
}

#endif /* GLFrame_h */
