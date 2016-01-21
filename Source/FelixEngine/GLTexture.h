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
   *
   */
  class GLTexture: public Texture
  {
  public:
    GLTexture(GLGraphicSystem *system): mGLSystem(system), mMipMapped(0), mFBOTexture(0) {}
    virtual ~GLTexture() {}
    
    void update()
    {
      if (loading())
      {
        if (load())
          setLoaded();
        else
          setNotLoading();
      }
    }
    
    void use(GLint index, const Sampler &sampler) const
    {
      glActiveTexture(GL_TEXTURE0 + index);
      glBindTexture(GL_TEXTURE_2D, mTextureId);
      setFilters(sampler);
    }
    
    void setBuffer(GLuint textureId, const ivec2 &size)
    {
      mTextureId = textureId;
      mSize = size;
      mFBOTexture = true;
      setLoaded();
    }
    void clearBuffer()
    {
      mTextureId = 0;
      mSize = ivec2();
      mFBOTexture = false;
      setUnloaded();
    }
    
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
        
        setFilters(mSampler);
        
        // Load the Image to the Texture
        const GLvoid *pixels = buffer.ptr();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)mSize.w, (GLint)mSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        // Generate the Mipmaps
        if (mSampler.mipMappingEnabled())
        {
          mMipMapped = true;
          glGenerateMipmap(GL_TEXTURE_2D);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        success = true;
      }
      
      if (success)
        setLoaded();
      else
        setUnloaded();
    }
    
    void unload()
    {
      if (!mFBOTexture && mTextureId)
      {
        glDeleteTextures(1, &mTextureId);
        mTextureId = 0;
      }
    }
    
    
    GLuint textureId() const {return mTextureId;}
    
  private:
    bool load()
    {
//      if (!loaded() && !mFBOTexture)
//        return loadImageFile(mFile);
      return false;
    }
    
//    bool loadImageFile(const std::string &file)
//    {
//      ImageRGBA image;
//      return ImageLoader::LoadImageFromFile(image, file) && loadImage(image);
//    }
//    
//    bool loadImage(const ImageRGBA &image)
//    {
//      if (!mTextureId)
//      {
//        glGenTextures(1, &mTextureId);
//        if (!mTextureId)
//        {
//          std::cerr << "failed to create GLTexture" << std::endl;
//          return false;
//        }
//      }
//      
//      mSize.w = image.width();
//      mSize.h = image.height();
//      
//      glBindTexture(GL_TEXTURE_2D, mTextureId);
//      
//      setFilters(mSampler);
//      
//      // Load the Image to the Texture
//      const GLvoid *pixels = image.ptr();
//      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)mSize.w, (GLint)mSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//      
//      // Generate the Mipmaps
//      if (mSampler.mipMappingEnabled())
//      {
//        mMipMapped = true;
//        glGenerateMipmap(GL_TEXTURE_2D);
//      }
//      glBindTexture(GL_TEXTURE_2D, 0);
//      
//      return true;
//    }
    
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
    GLint getMagFilter(const Sampler &sampler) const
    {
      return sampler.magFilter() == SAMPLER_FILTER_LINEAR ? GL_LINEAR : GL_NEAREST;
    }
    
    bool  isPowerOfTwo(size_t value) const {return (value != 0) && !(value & (value - 1));}
    bool  isPowerOfTwo() const {return !isPowerOfTwo(mSize.w) || !isPowerOfTwo(mSize.h);}
    
  private:
    GLuint mTextureId;
    bool   mMipMapped;
    bool   mFBOTexture;
    GLGraphicSystem *mGLSystem;
  };
}

#endif /* GLTexture_h */
