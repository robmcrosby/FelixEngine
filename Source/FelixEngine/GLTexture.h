//
//  GLTexture.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GLTexture_h
#define GLTexture_h

#include "GLGraphicSystem.h"


namespace fx
{
  /**
   * Resource that manages an OpenGL texture.
   */
  class GLTexture: public Texture
  {
  public:
    /**
     * Constructor takes a pointer to an OpenGL Graphics System.
     *
     * @param system Pointer to the GLGraphicSystem
     */
    GLTexture(GLGraphicSystem *system): mGLSystem(system), mMipMapped(0), mFBOTexture(0) {}
    
    /**
     * Destructor
     */
    virtual ~GLTexture() {unload();}
    
    /**
     * Applies the texture to the given index and filter settings in the Sampler
     * to the active shader.
     */
    void use(GLint index, const Sampler &sampler) const
    {
      glActiveTexture(GL_TEXTURE0 + index);
      glBindTexture(GL_TEXTURE_2D, mTextureId);
      setFilters(sampler);
    }
    
    /**
     * Sets the texture to part of a FBO.
     *
     * @param textureId intenger for the texture handle of the FBO
     * @param size 2d intenger vector for the size of the texture
     */
    void setBuffer(GLuint textureId, const ivec2 &size)
    {
      mTextureId = textureId;
      mSize = size;
      mFBOTexture = true;
      setLoaded();
    }
    
    /**
     * Removes the refrence to a FBO.
     */
    void clearBuffer()
    {
      if (mFBOTexture)
      {
        mTextureId = 0;
        mSize = ivec2();
        mFBOTexture = false;
        setUnloaded();
      }
    }
    
    /**
     * Uploads the contents of the given Buffer Map to video memory.
     *
     * @param bufferMap refrence to the buffer map to upload.
     */
    void uploadBufferMap(const BufferMap &bufferMap)
    {
      bool success = false;
      const Buffer &buffer = bufferMap["data"];
      
      unload();
      if (!mTextureId)
        glGenTextures(1, &mTextureId);
      if (!mTextureId)
        std::cerr << "Error: creating OpenGL Texture" << std::endl;
      else
      {
        mSize.w = (int)buffer.width();
        mSize.h = (int)buffer.height();
        
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        
        // Load the Image to the Texture
        const GLvoid *pixels = buffer.ptr();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)mSize.w, (GLint)mSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        // TODO: Implement Mipmap loading/creation
//        // Generate the Mipmaps
//        if (mSampler.mipMappingEnabled())
//        {
//          mMipMapped = true;
//          glGenerateMipmap(GL_TEXTURE_2D);
//        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        success = true;
      }
      setLoaded(success);
    }
    
    /**
     * Deletes the texture from video memory if there and is not a FBO texture.
     */
    void unload()
    {
      if (!mFBOTexture && mTextureId)
      {
        glDeleteTextures(1, &mTextureId);
        mTextureId = 0;
      }
      setLoaded(false);
    }
    
    /**
     * OpenGL Texture handle
     */
    GLuint textureId() const {return mTextureId;}
    
  private:
    
    /**
     * Sets the Texture Minification and Magnification filters from the given Sampler.
     *
     * @param sampler Sampler refrence to get the filter settings from.
     */
    void  setFilters(const Sampler &sampler) const
    {
      if (isPowerOfTwo())
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }
      
      // Set the Texture Filters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getMinFilter(sampler));
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getMagFilter(sampler));
    }
    
    /**
     * Gets the Minification filter from the given Sampler.
     *
     * @param sampler Sampler refrence to get the Minification settings from.
     * @return Valid OpenGL Minification intenger value
     */
    GLint getMinFilter(const Sampler &sampler) const
    {
      if (mMipMapped && sampler.mipMappingEnabled())
      {
        if (sampler.mipFilter() == SAMPLER_FILTER_LINEAR)
          return sampler.minFilter() == SAMPLER_FILTER_LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
        return sampler.minFilter() == SAMPLER_FILTER_LINEAR  ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
      }
      return sampler.minFilter() == SAMPLER_FILTER_LINEAR ? GL_LINEAR : GL_NEAREST;
    }
    
    /**
     * Gets the Magnification filter from the given Sampler.
     *
     * @param sampler Sampler refrence to get the Magnification settings from.
     * @return Valid OpenGL Magnification intenger value
     */
    GLint getMagFilter(const Sampler &sampler) const
    {
      return sampler.magFilter() == SAMPLER_FILTER_LINEAR ? GL_LINEAR : GL_NEAREST;
    }
    
    /**
     * Determines if the given intenger value is a power of two.
     *
     * @param value Intenger value to determine if is a power of two.
     * @return true if value is a power of tow or false otherwise.
     */
    bool  isPowerOfTwo(size_t value) const {return (value != 0) && !(value & (value - 1));}
    
    /**
     * Determines if values in size are powers of two.
     *
     * @return true if both width and height are powers of two or false otherwise.
     */
    bool  isPowerOfTwo() const {return !isPowerOfTwo(mSize.w) || !isPowerOfTwo(mSize.h);}
    
  private:
    GLuint mTextureId;  /**< OpenGL texture handle */
    bool   mMipMapped;  /**< Boolean if the texture is Mip Mapped */
    bool   mFBOTexture; /**< Boolean if the texture handle is part of a FBO */
    
    GLGraphicSystem *mGLSystem; /**< Pointer to the GLGraphicsSystem that owns this texture */
  };
}

#endif /* GLTexture_h */
