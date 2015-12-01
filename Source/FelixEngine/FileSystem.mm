//
//  FileSystem.mm
//  FelixEngine
//
//  Created by Robert Crosby on 12/1/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "FileSystem.h"

#include <fstream>
#include <SDL2/SDL.h>

#import <Foundation/Foundation.h>


using namespace fx;
using namespace std;


std::string FileSystem::GetLocalPath()
{
  NSBundle* mainBundle = [NSBundle mainBundle];
  if (mainBundle != nil)
  {
    NSString* path = [mainBundle resourcePath];
    return [path UTF8String] + std::string("/");
  }
  return "";
}


bool FileSystem::LoadText(string &buffer, const string &filePath)
{
  bool success = false;
  fstream fileIn;
  
  fileIn.open(filePath.c_str(), ios::in);
  if (fileIn.is_open())
  {
    buffer = string((istreambuf_iterator<char>(fileIn)), istreambuf_iterator<char>());
    fileIn.close();
    success = true;
  }
  else
    cerr << "Error Reading Text File: " << filePath << endl;
  return success;
}

string FileSystem::LoadText(const string &filePath)
{
  string contents;
  LoadText(contents, filePath);
  return contents;
}

long FileSystem::Read(unsigned int *buffer, long size, std::istream &is)
{
  is.read((char*)buffer, size*sizeof(unsigned int));
  
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
  for (long i = 0; i < size; ++i, ++buffer)
    *buffer = SDL_Swap32(*buffer);
#endif
  
  return is.gcount();
}
