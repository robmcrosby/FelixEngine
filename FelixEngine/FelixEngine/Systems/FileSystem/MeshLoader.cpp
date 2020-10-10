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

bool MeshLoader::loadFromBinaryFile(VertexMeshData &mesh, const string &file) {
  ifstream meshFile;
  meshFile.open(file, ios::in | ios::binary);
  if (meshFile.is_open())
  {
    bool success = loadFromBinarySteam(mesh, meshFile);
    meshFile.close();
    return success;
  }
  
  cerr << "Error Opening Mesh File: " << file << endl;
  return false;
}

bool MeshLoader::loadFromBinarySteam(VertexMeshData &mesh, std::istream &is) {
  bool success = true;
  int primitiveType = 0;
  int numSubMeshes = 0;
  
  if (readB(primitiveType, is) == sizeof(int) && readB(numSubMeshes, is) == sizeof(int)) {
    // Determin the Primitive Type
    mesh.primitiveType = primitiveType & 1 ? VERTEX_TRIANGLE_STRIP : VERTEX_TRIANGLES;
    
    // Read the Sub Meshes
    mesh.subMeshes.resize(numSubMeshes);
    success = readB((int*)&mesh.subMeshes[0], numSubMeshes*2, is) == sizeof(int)*numSubMeshes*2;
    
    int numVertices, numBuffers;
    if (success && readB(numVertices, is) == sizeof(int) && readB(numBuffers, is) == sizeof(int)) {
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
  
  if (readB(compSize, is) == sizeof(int))
  {
    char name[NAME_BUFFER_SIZE];
    
    is.read(name, sizeof(name));
    if (is.gcount() == NAME_BUFFER_SIZE && name[0] != '\0')
    {
      int bufferSize = vertexCount*compSize;
      VertexBuffer &buffer = mesh.buffers[name];
      buffer.resize(bufferSize);
      return readB(&buffer[0], bufferSize, is) == sizeof(float)*bufferSize;
    }
  }
  return false;
}

bool MeshLoader::readIndicesBinaryStream(VertexMeshData &mesh, std::istream &is) {
  int numIndices;
  if (readB(numIndices, is) == sizeof(int)) {
    mesh.indices.resize(numIndices);
    return readB(&mesh.indices[0], numIndices, is) == sizeof(int)*numIndices;
  }
  
  cerr << "Error Reading Index Buffer" << endl;
  return false;
}

bool MeshLoader::loadFromUsdzFile(VertexMeshData &mesh, const string &file) {
  ifstream usdzFile;
  usdzFile.open(file, ios::in | ios::binary);
  if (usdzFile.is_open())
  {
    bool success = loadFromUsdzSteam(mesh, usdzFile);
    usdzFile.close();
    return success;
  }
  
  cerr << "Error Opening Usdz File: " << file << endl;
  return false;
}

bool MeshLoader::loadFromUsdzSteam(VertexMeshData &mesh, std::istream &is) {
  // Find End of Central Directory Record
  int signature = 0;
  is.seekg(-22, is.end);
  size_t pos = is.tellg();
  while (signature != 0x504b0506) {
    is.seekg(pos--);
    if (pos <= 0 || readB(signature, is) != sizeof(int)) {
      cerr << "Error Finding End of Central Directory Record of Zip File" << endl;
      return false;
    }
  }
  
  pos = is.tellg();
  pos += 4;
  
  // Read the Number of Entries
  int numEntries;
  if (readL(numEntries, is) != sizeof(int) || readL(numEntries, is) != sizeof(int)) {
    cerr << "Error Getting Number of Entries of Zip File" << endl;
    return false;
  }
  numEntries &= 0xffff;
  //cout << "numEntries: " << numEntries << endl;
  
  // Read the Centeral Directory Length and Offset
  int cdLength, cdOffset;
  if (readL(cdLength, is) != sizeof(int) || readL(cdOffset, is) != sizeof(int)) {
    cerr << "Error Finding Central Directory Record of Zip File" << endl;
    return false;
  }
  //cout << "cdLength: " << cdLength << endl;
  //cout << "cdOffset: " << cdOffset << endl;
  
  
  
  
  pos = cdOffset;
  is.seekg(pos);
  readB(signature, is);
  //cout << "signature: " << std::hex << signature << endl;
  
  pos = cdOffset + 28;
  is.seekg(pos);
  
  int strLength;
  readL(strLength, is);
  strLength &= 0xffff;
  cout << "strLength: " << strLength << endl;
  
  pos = cdOffset + 36;
  is.seekg(pos);
  
  int headerOffset;
  readL(headerOffset, is);
  cout << "headerOffset: " << headerOffset << endl;
  
  pos = cdOffset + 46;
  is.seekg(pos);
  
  char strBuffer[256];
  is.read(strBuffer, strLength);
  cout << "name: " << strBuffer << endl;
  
  
  return false;
}
