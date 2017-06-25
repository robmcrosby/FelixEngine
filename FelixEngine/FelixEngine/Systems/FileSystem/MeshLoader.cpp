//
//  MeshLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/24/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MeshLoader.h"
#include <fstream>


using namespace fx;
using namespace std;

bool MeshLoader::loadFromBinaryFile(MeshData &mesh, const std::string &file) {
  std::ifstream meshFile;
  meshFile.open(file, std::ios::in | std::ios::binary);
  if (meshFile.is_open())
  {
    bool success = loadFromBinarySteam(mesh, meshFile);
    meshFile.close();
    return success;
  }
  
  std::cerr << "Error Opening Mesh File: " << file << std::endl;
  return false;
}

bool MeshLoader::loadFromBinarySteam(MeshData &mesh, std::istream &is) {
  int primitiveType = 0;
  int numSubMeshes = 0;
  
  if (FileSystem::read(primitiveType, is) == sizeof(int) && FileSystem::read(numSubMeshes, is) == sizeof(int)) {
    cout << "Primative Type: " << primitiveType << " Submeshes: " << numSubMeshes << endl;
  }
//  {
//    bufferMap.setFlags(GetVertexPrimitive(primitiveType));
//    
//    // Load the Sub-Mesh Information
//    Buffer &subMeshBuffer = bufferMap.getBuffer("SubMeshes", BUFFER_RANGES);
//    subMeshBuffer.resize(VAR_INT_2, numSubMeshes);
//    FileSystem::Read((unsigned int*)subMeshBuffer.ptr(), numSubMeshes*2, is);
//    
//    int numVertices, numBuffers;
//    if (success && FileSystem::Read(numVertices, is) == sizeof(int) && FileSystem::Read(numBuffers, is) == sizeof(int))
//    {
//      for (int i = 0; i < numBuffers && success; ++i)
//        success |= ReadBufferFromStream(bufferMap, numVertices, is);
//    }
//    else
//      success = false;
//    
//    if (primitiveType == 8 || primitiveType == 9)
//    {
//      int numIndices;
//      if (success && FileSystem::Read(numIndices, is) == sizeof(int))
//      {
//        Buffer &indexBuffer = bufferMap.getBuffer("Indices", BUFFER_INDICES);
//        indexBuffer.resize(VAR_INT, numIndices);
//        FileSystem::Read((unsigned int*)indexBuffer.ptr(), numIndices, is);
//      }
//      else
//        success = false;
//    }
//  }
//  else
//    success = false;
//  
//  return success;
  return false;
}
