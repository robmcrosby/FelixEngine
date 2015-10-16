//
//  GLTexture.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "GLTexture.h"
#include "GLGraphicSystem.h"
#include "ImageLoader.h"


using namespace fx;
using namespace std;


GLTexture::GLTexture(GLGraphicSystem *system): mGLSystem(system), mMipMapped(0), mFBOTexture(0)
{
}

GLTexture::~GLTexture()
{
}

void GLTexture::reload()
{
  GraphicResource::reload();
  if (mGLSystem)
    mGLSystem->loadOnNextUpdate();
}

bool GLTexture::load()
{
  if (!mLoaded && !mFBOTexture)
    mLoaded = loadImageFile(mFile);
  return mLoaded;
}

bool GLTexture::loadImageFile(const std::string &file)
{
  ImageRGBA image;
  return ImageLoader::LoadImageFromFile(image, file) && loadImage(image);
}

bool GLTexture::loadImage(const ImageRGBA &image)
{
  if (!mTextureId)
  {
    glGenTextures(1, &mTextureId);
    if (!mTextureId)
    {
      cerr << "failed to create GLTexture" << endl;
      return false;
    }
  }
  
  mSize.w = image.width();
  mSize.h = image.height();
  
  glBindTexture(GL_TEXTURE_2D, mTextureId);
  
  setFilters(mSampler);
  
  // Load the Image to the Texture
  const GLvoid *pixels = image.ptr();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)mSize.w, (GLint)mSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  
  // Generate the Mipmaps
  if (mSampler.mipMappingEnabled())
  {
    mMipMapped = true;
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  
  glBindTexture(GL_TEXTURE_2D, 0);
  
  return true;
}