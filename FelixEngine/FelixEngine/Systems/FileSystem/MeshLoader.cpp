//
//  MeshLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/24/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MeshLoader.h"
#include <fstream>

#define NAME_BUFFER_SIZE 32


using namespace fx;
using namespace std;

bool MeshLoader::loadFromBinaryFile(VertexMeshData &mesh, const std::string &file) {
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

bool MeshLoader::loadFromBinarySteam(VertexMeshData &mesh, std::istream &is) {
  bool success = true;
  int primitiveType = 0;
  int numSubMeshes = 0;
  
  if (FileSystem::read(primitiveType, is) == sizeof(int) && FileSystem::read(numSubMeshes, is) == sizeof(int)) {
    // Determin the Primitive Type
    mesh.primitiveType = primitiveType & 1 ? VERTEX_TRIANGLE_STRIP : VERTEX_TRIANGLES;
    
    // Read the Sub Meshes
    mesh.subMeshes.resize(numSubMeshes);
    success = FileSystem::read((int*)&mesh.subMeshes[0], numSubMeshes*2, is) == sizeof(int)*numSubMeshes*2;
    
    int numVertices, numBuffers;
    if (success && FileSystem::read(numVertices, is) == sizeof(int) && FileSystem::read(numBuffers, is) == sizeof(int)) {
      mesh.totalVertices = numVertices;
      
      // Read the Vertex Buffers
      for (int i = 0; i < numBuffers && success; ++i)
        success |= readBufferBinaryStream(mesh, numVertices, is);
      
      if (success && primitiveType & 8) {
        success = readIndicesBinaryStream(mesh, is);
      }
    }
    else {
      cerr << "Error Reading Number of Vertices and Number of Buffers" << endl;
      success = false;
    }
  }
  else {
    cerr << "Error Reading Primitive Type and Number of Submeshes" << endl;
    success = false;
  }
  return success;
}

bool MeshLoader::readBufferBinaryStream(VertexMeshData &mesh, int vertexCount, std::istream &is) {
  int compSize = 0;
  
  if (FileSystem::read(compSize, is) == sizeof(int))
  {
    char name[NAME_BUFFER_SIZE];
    
    is.read(name, sizeof(name));
    if (is.gcount() == NAME_BUFFER_SIZE && name[0] != '\0')
    {
      int bufferSize = vertexCount*compSize;
      VertexBuffer &buffer = mesh.buffers[name];
      buffer.resize(bufferSize);
      return FileSystem::read(&buffer[0], bufferSize, is) == sizeof(float)*bufferSize;
    }
  }
  return false;
}

bool MeshLoader::readIndicesBinaryStream(VertexMeshData &mesh, std::istream &is) {
  int numIndices;
  if (FileSystem::read(numIndices, is) == sizeof(int)) {
    mesh.indices.resize(numIndices);
    return FileSystem::read(&mesh.indices[0], numIndices, is) == sizeof(int)*numIndices;
  }
  
  cerr << "Error Reading Index Buffer" << endl;
  return false;
}
