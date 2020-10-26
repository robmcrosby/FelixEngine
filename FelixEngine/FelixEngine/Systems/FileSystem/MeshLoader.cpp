//
//  MeshLoader.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/24/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "MeshLoader.h"
#include "USDCrate.h"
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

bool MeshLoader::loadFromCrateFile(VertexMeshData &mesh, USDCrate &crate) {
  mesh.primitiveType = VERTEX_TRIANGLES;
  
  crate.open();
  
  crate.printUSD();
  
  StringVector meshPaths = crate.meshPaths();
  for (auto path = meshPaths.begin(); path != meshPaths.end(); ++path) {
    IntVector counts, indices;
    crate.getArray(counts, *path, "faceVertexCounts");
    crate.getArray(indices, *path, "faceVertexIndices");
    
    //vector<vec3> positions, normals;
    //crate.getArray(positions, *path, "points");
    //crate.getArray(normals, *path, "primvars:normals");
    
    int i = 0, offset = (int)mesh.buffers["position"].size()/3;
    ivec2 range((int)mesh.indices.size(), 0);
    
    for (auto count = counts.begin(); count != counts.end(); ++count) {
      for (int j = i+1; j < i+*count-1;) {
        mesh.indices.push_back(offset + indices[i]);
        mesh.indices.push_back(offset + indices[j]);
        mesh.indices.push_back(offset + indices[++j]);
      }
      i += *count;
    }
    range.end = (int)mesh.indices.size();
    mesh.subMeshes.push_back(range);
    
    crate.appendBuffer(mesh.buffers["position"], *path, "points");
    crate.appendBuffer(mesh.buffers["normal"], *path, "primvars:normals");
    
//    cout << "faceVertexCounts: {";
//    for (auto itr = counts.begin(); itr != counts.end(); ++itr)
//      cout << *itr << ", ";
//    cout << "}" << endl;
//
//    cout << "faceVertexIndices: {";
//    for (auto itr = indices.begin(); itr != indices.end(); ++itr)
//      cout << *itr << ", ";
//    cout << "}" << endl;
  }
  mesh.totalVertices = (int)mesh.buffers["position"].size()/3;
  
//  cout << "mesh.indices: {";
//  for (auto itr = mesh.indices.begin(); itr != mesh.indices.end(); ++itr)
//    cout << *itr << ", ";
//  cout << "}" << endl;
  
  crate.close();
  
  //cout << "loadFromCrateFile" << endl;
  return false;
}
