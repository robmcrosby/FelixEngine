//
//  MeshBuilder.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 3/14/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "MeshBuilder.h"
#include "FileSystem.h"


using namespace fx;
using namespace std;

VertexMeshPtr MeshBuilder::createFromFile(const std::string &file) {
  VertexMeshData meshData;
  FileSystem::loadMesh(meshData, file);
  
  VertexMeshPtr mesh = Graphics::getInstance().createVertexMesh();
  mesh->load(meshData);
  return mesh;
}

VertexMeshPtr MeshBuilder::createCube(float size) {
  VertexMeshData meshData;
  
  size /= 2.0f;
  float positions[] = {
    -size, -size, 0.0f,
     size,  size, 0.0f,
    -size,  size, 0.0f,
    -size, -size, 0.0f,
     size, -size, 0.0f,
     size,  size, 0.0f,
  };
  float normals[] = {
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f
  };
  
  meshData.buffers["position"].assign(positions, positions+18);
  meshData.buffers["normal"].assign(normals, normals+18);
  meshData.totalVertices = 6;
  meshData.subMeshes.push_back(ivec2(0, 6));
  
//  VertexBuffer &pos = meshData.buffers["position"];
//  VertexBuffer &nor = meshData.buffers["normal"];
  
  
  VertexMeshPtr mesh = Graphics::getInstance().createVertexMesh();
  mesh->load(meshData);
  return mesh;
}
