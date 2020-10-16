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

size_t FileSystem::readB(uint16_t *buffer, size_t count, istream &is) {
  is.read((char*)buffer, count*sizeof(uint16_t));
  // Convert Endianess
  for (size_t i = 0; i < count; ++i, ++buffer)
    *buffer = EndianU16_BtoN(*buffer);
  return is.gcount();
}

size_t FileSystem::readL(uint16_t *buffer, size_t count, istream &is) {
  is.read((char*)buffer, count*sizeof(uint16_t));
  // Convert Endianess
  for (size_t i = 0; i < count; ++i, ++buffer)
    *buffer = EndianU16_LtoN(*buffer);
  return is.gcount();
}

size_t FileSystem::readB(uint32_t *buffer, size_t count, istream &is) {
  is.read((char*)buffer, count*sizeof(uint32_t));
  // Convert Endianess
  for (size_t i = 0; i < count; ++i, ++buffer)
    *buffer = EndianU32_BtoN(*buffer);
  return is.gcount();
}

size_t FileSystem::readL(uint32_t *buffer, size_t count, istream &is) {
  is.read((char*)buffer, count*sizeof(uint32_t));
  // Convert Endianess
  for (size_t i = 0; i < count; ++i, ++buffer)
    *buffer = EndianU32_LtoN(*buffer);
  return is.gcount();
}

size_t FileSystem::readB(uint64_t *buffer, size_t count, istream &is) {
  is.read((char*)buffer, count*sizeof(uint64_t));
  // Convert Endianess
  for (size_t i = 0; i < count; ++i, ++buffer)
    *buffer = EndianU64_BtoN(*buffer);
  return is.gcount();
}

size_t FileSystem::readL(uint64_t *buffer, size_t count, istream &is) {
  is.read((char*)buffer, count*sizeof(uint64_t));
  // Convert Endianess
  for (size_t i = 0; i < count; ++i, ++buffer)
    *buffer = EndianU64_LtoN(*buffer);
  return is.gcount();
}
