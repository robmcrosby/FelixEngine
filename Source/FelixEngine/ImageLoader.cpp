//
//  ImageLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//


#include "ImageLoader.h"
#include "FileSystem.h"
#include "FreeImage.h"

using namespace fx;
using namespace std;

bool ImageLoader::LoadImageFromFile(ImageRGBA &image, const std::string &filePath)
{
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filePath.c_str() ,0);
  FIBITMAP* fimage = FreeImage_Load(format, filePath.c_str());
  if (!fimage)
    return false;
  
  FIBITMAP* temp = fimage;
  fimage = FreeImage_ConvertTo32Bits(fimage);
  FreeImage_Unload(temp);
  
  unsigned int width  = FreeImage_GetWidth(fimage);
  unsigned int height = FreeImage_GetHeight(fimage);
  
  //cout << "Size: " << width << " by " << height << endl;
  
  image.resize(width, height);
  
  // FreeImage loads in BGRA, Change to RGBA format.
  char* pixeles = (char*)FreeImage_GetBits(fimage);
  vector<RGBA>::iterator itr = image.data().begin();
  for(int i = 0; i < width * height; ++i, ++itr){
    itr->red   = pixeles[i*4+2];
    itr->green = pixeles[i*4+1];
    itr->blue  = pixeles[i*4+0];
    itr->alpha = pixeles[i*4+3];
  }
  
  FreeImage_Unload(fimage);
  
  return true;
}

bool ImageLoader::LoadImageFromFile(BufferMap &textureMap, const std::string &file)
{
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(file.c_str() ,0);
  FIBITMAP* fimage = FreeImage_Load(format, file.c_str());
  if (!fimage)
    return false;
  
  FIBITMAP* temp = fimage;
  fimage = FreeImage_ConvertTo32Bits(fimage);
  FreeImage_Unload(temp);
  
  unsigned int width  = FreeImage_GetWidth(fimage);
  unsigned int height = FreeImage_GetHeight(fimage);
  
  // Setup the TextureMap with a single texture.
  textureMap.clear();
  textureMap.setType(BUFFER_MAP_TEXTURES);
  Buffer &textureBuffer = textureMap.getBuffer("Main", BUFFER_TEXTURE);
  textureBuffer.resize(VAR_CHAR_4, width, height);
  
  char *pixeles = (char*)FreeImage_GetBits(fimage);
  RGBA *texture = (RGBA*)textureBuffer.ptr();
  for (int i = 0; i < width*height; ++i)
  {
    texture[i].red   = pixeles[i*4+2];
    texture[i].green = pixeles[i*4+1];
    texture[i].blue  = pixeles[i*4+0];
    texture[i].alpha = pixeles[i*4+3];
  }
  
  FreeImage_Unload(fimage);
  return true;
}

