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
#include "Image.h"


namespace fx
{
  /**
   *
   */
  class GLTexture: public Texture
  {
  public:
    GLTexture(GLGraphicSystem *system);
    virtual ~GLTexture();
    
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
    
    void use(GLint index, const Sampler &sampler) const
    {
      glActiveTexture(GL_TEXTURE0 + index);
      glBindTexture(GL_TEXTURE_2D, mTextureId);
      setFilters(sampler);
    }
    
    GLuint useWithBuffer(const ivec2 &size)
    {
      if (!mTextureId)
        glGenTextures(1, &mTextureId);
      mSize = size;
      mFBOTexture = true;
      setLoaded();
      return mTextureId;
    }
    GLuint textureId() const {return mTextureId;}
    
  private:
    bool load();
    bool loadImageFile(const std::string &file);
    bool loadImage(const ImageRGBA &image);
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
