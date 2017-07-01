//
//  FileSystem.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/25/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "FileSystem.h"
#include <Endian.h>


using namespace fx;
using namespace std;


string FileSystem::getFilePostfix(const std::string &fileName)
{
  size_t loc = fileName.rfind(".");
  if (loc != std::string::npos)
    return fileName.substr(loc+1);
  return "";
}

string FileSystem::getFileName(const std::string &file)
{
  string::size_type start, end, npos = string::npos;
  
  start = file.find_last_of('/');
  if (start == npos)
    start = 0;
  else
    start++;
  
  end = file.find_last_of('.');
  
  if (end != npos && start < end)
    return file.substr(start, end-start);
  
  cerr << "Unable to get filename from path: " << file << endl;
  return "";
}

size_t FileSystem::read(uint32_t *buffer, size_t size, istream &is) {
  is.read((char*)buffer, size*sizeof(uint32_t));
  
  // Convert Endianess
  for (size_t i = 0; i < size; ++i, ++buffer)
    *buffer = EndianU32_BtoN(*buffer);
  
  return is.gcount();
}
