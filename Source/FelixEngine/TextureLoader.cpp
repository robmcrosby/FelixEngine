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


bool TextureLoader::LoadTextureFromXml(Buffer &texture, const XMLTree::Node &node)
{
  if (node.hasAttribute("file"))
    return LoadTextureFromFile(texture, node.tree()->path() + node.attribute("file"));
  return false;
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
  
  texture.setBufferType(BUFFER_TEXTURE);
  texture["size"] = ivec2(width, height);
  Buffer &buffer = texture.addBuffer("data", BUFFER_TEXTURE);
  
  buffer.resize(VAR_CHAR_4, width, height);
  
  char *pixeles = (char*)FreeImage_GetBits(fimage);
  RGBA *tex = (RGBA*)buffer.ptr();
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

