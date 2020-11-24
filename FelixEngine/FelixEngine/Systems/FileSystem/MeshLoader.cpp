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


typedef map<long, int> IndexMap;

struct Vec3Set {
  IntVector indices;
  vector<vec3> buffer;
  int offset;
};

struct Vec2Set {
  IntVector indices;
  vector<vec2> buffer;
  int offset;
};
typedef map<string, Vec2Set> Vec2SetMap;

void addVertex(VertexMeshData &mesh, IndexMap &indexMap, int index, Vec3Set &faces, Vec3Set &normals, Vec2SetMap &uvs) {
  long key = faces.indices[index];
  key |= normals.indices[index] << normals.offset;
  for (auto &uv : uvs)
    key |= uv.second.indices[index] << uv.second.offset;
  
  if (indexMap.count(key) > 0)
    mesh.indices.push_back(indexMap.at(key));
  else {
    mesh.indices.push_back((int)mesh.buffers["position"].size()/3);
    indexMap[key] = mesh.indices.back();
    
    vec3 &pos = faces.buffer[faces.indices[index]];
    mesh.buffers["position"].push_back(pos.x);
    mesh.buffers["position"].push_back(pos.y);
    mesh.buffers["position"].push_back(pos.z);
    
    vec3 &normal = normals.buffer[normals.indices[index]];
    mesh.buffers["normal"].push_back(normal.x);
    mesh.buffers["normal"].push_back(normal.y);
    mesh.buffers["normal"].push_back(normal.z);
    
    for (auto &uv : uvs) {
      vec2 &coord = uv.second.buffer[uv.second.indices[index]];
      mesh.buffers[uv.first].push_back(coord.x);
      mesh.buffers[uv.first].push_back(coord.y);
    }
  }
}



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

bool MeshLoader::loadFromCrateFile(VertexMeshData &mesh, const USDCrate &crate) {
  bool success = true;
  //cout << crate << endl;
  
  StringVector paths = crate.meshPaths();
  for (auto &path : paths)
    success &= loadFromCrateFile(mesh, crate.getItem(path));
  return success;
}

bool MeshLoader::loadFromCrateFile(VertexMeshData &mesh, const USDItem &item) {
  if (item.openCrate()) {
    mesh.primitiveType = VERTEX_TRIANGLES;
    
    // Get Face Counts
    IntVector faceCounts;
    item.getAttributeArray(faceCounts, "faceVertexCounts");
    
    // Get Face Vertices
    Vec3Set faces;
    item.getAttributeArray(faces.indices, "faceVertexIndices");
    item.getAttributeArray(faces.buffer, "points");
    faces.offset = 0;
    
    // Get Normals
    Vec3Set normals;
    if (item.getAttributeArray(normals.indices, "primvars:normals:indices"))
      item.getAttributeArray(normals.buffer, "primvars:normals");
    else {
      // Create Normals if Not Avalible
      int i = 0;
      for (int faceCount : faceCounts) {
        for (int j = 0; j < faceCount; ++j)
          normals.indices.push_back((int)normals.buffer.size());
        vec3 v1 = faces.buffer.at(faces.indices.at(i)) - faces.buffer.at(faces.indices.at(i+1));
        vec3 v2 = faces.buffer.at(faces.indices.at(i)) - faces.buffer.at(faces.indices.at(i+2));
        normals.buffer.push_back(v1.cross(v2).normalized());
        i += faceCount;
      }
    }
    normals.offset = (int)ceil(log2((double)faces.buffer.size()));
    
    // Get UVs
    int offset = normals.offset + (int)ceil(log2((double)normals.buffer.size()));
    StringVector uvNames = item.getUVNames();
    Vec2SetMap uvMap;
    for (int i = 0; i < uvNames.size(); ++i) {
      stringstream ss;
      ss << "uv" << i;
      Vec2Set &uv = uvMap[ss.str()];
      item.getAttributeArray(uv.indices, "primvars:" + uvNames[i] + ":indices");
      item.getAttributeArray(uv.buffer, "primvars:" + uvNames[i]);
      uv.offset = offset;
      offset += (int)ceil(log2((double)uv.buffer.size()));
    }
    if (uvMap.size() == 0) {
      // Create UVs if None Found
      Vec2Set &uv = uvMap["uv0"];
      uv.indices.resize(faces.indices.size(), 0);
      uv.buffer.push_back(vec2(0.0f, 0.0f));
      uv.offset = 0;
    }
    
    int i = 0;
    IndexMap indexMap;
    ivec2 range((int)mesh.indices.size(), 0);

    for (int faceCount : faceCounts) {
      for (int j = i+1; j < i+faceCount-1;) {
        addVertex(mesh, indexMap, i, faces, normals, uvMap);
        addVertex(mesh, indexMap, j, faces, normals, uvMap);
        addVertex(mesh, indexMap, ++j, faces, normals, uvMap);
      }
      i += faceCount;
    }
    range.end = (int)mesh.indices.size();
    mesh.subMeshes.push_back(range);

    mesh.totalVertices = (int)mesh.buffers["position"].size()/3;
    
    item.closeCrate();
    return true;
  }
  return false;;
}
