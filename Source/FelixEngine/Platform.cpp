//
//  Platform.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/5/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "Platform.h"
#include <fstream>
#include <SDL2/SDL.h>


using namespace fx;
using namespace std;


std::string Platform::LoadTextFile(const string &filePath)
{
  fstream fileIn;
  string txtStr;
  
  fileIn.open(filePath.c_str(), ios::in);
  if (fileIn.is_open())
  {
    txtStr = string((istreambuf_iterator<char>(fileIn)), istreambuf_iterator<char>());
    fileIn.close();
  }
  else
    cerr << "Error Reading Text File: " << filePath << endl;
  
  return txtStr;
}

long Platform::ReadUints(unsigned int *buffer, long size, std::istream &is)
{
  is.read((char*)buffer, size*sizeof(int));
  
  #if SDL_BYTEORDER == SDL_LIL_ENDIAN
  for (long i = 0; i < size; ++i, ++buffer)
    *buffer = SDL_Swap32(*buffer);
  #endif
  
  return is.gcount();
}

uint Platform::GetTimeStamp()
{
  return SDL_GetTicks();
}
