//
//  TextureLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 1/19/16.
//  Copyright © 2016 Robert Crosby. All rights reserved.
//

#include "TextureLoader.h"
#include "BufferMap.h"
#include "FileSystem.h"
#include "FreeImage.h"

using namespace fx;
using namespace std;


bool TextureLoader::LoadTexturesFromXml(BufferMap &textures, const XMLTree::Node &node)
{
  bool success = true;
  string path = node.tree()->path();
  
  textures.clear();
  textures.setType(BUFFER_MAP_TEXTURES);
  
  for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
  {
    Buffer &texture = textures.addBuffer((*itr)->attribute("name"), BUFFER_TEXTURE);
    success &= LoadTextureFromFile(texture, path+(*itr)->attribute("file"));
  }
  return success;
}

bool TextureLoader::LoadTextureFromFile(Buffer &texture, const string &file)
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
  
  texture.resize(VAR_CHAR_4, width, height);
  
  char *pixeles = (char*)FreeImage_GetBits(fimage);
  RGBA *tex = (RGBA*)texture.ptr();
  for (int i = 0; i < width*height; ++i)
  {
    tex[i].red   = pixeles[i*4+2];
    tex[i].green = pixeles[i*4+1];
    tex[i].blue  = pixeles[i*4+0];
    tex[i].alpha = pixeles[i*4+3];
  }
  
  FreeImage_Unload(fimage);
  return true;
}

