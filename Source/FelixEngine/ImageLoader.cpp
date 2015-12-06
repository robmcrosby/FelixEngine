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

bool ImageLoader::LoadImageFromFile(ImageRGBA &image, const std::string &file)
{
  string filePath = FileSystem::GetResourcesPath() + file;
  
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

