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

VertexMeshPtr MeshBuilder::createPlane() {
  float positionBuffer[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f
  };
  float normalBuffer[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
  };
  float uvBuffer[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  
  // Create the Mesh and set the buffers
  VertexMeshPtr mesh = Graphics::getInstance().createVertexMesh();
  mesh->setVertexBuffer("position", 3, 4, positionBuffer);
  mesh->setVertexBuffer("normal", 3, 4, normalBuffer);
  mesh->setVertexBuffer("uv0", 3, 4, uvBuffer);
  mesh->setPrimativeType(VERTEX_TRIANGLE_STRIP);
  
  // Load and return the mesh
  if (!mesh->loadBuffers()) {
    cerr << "Error Loading UV Plane" << endl;
    assert(false);
  }
  return mesh;
}

VertexMeshPtr MeshBuilder::createCube(float size) {
  VertexMeshData meshData;
  
  vec2 seq[] = {
    {-1.0f, -1.0f}, { 1.0f,  1.0f}, {-1.0f,  1.0f},
    {-1.0f, -1.0f}, { 1.0f, -1.0f}, { 1.0f,  1.0f},
    {-1.0f, -1.0f}, {-1.0f,  1.0f}, { 1.0f,  1.0f},
    {-1.0f, -1.0f}, { 1.0f,  1.0f}, { 1.0f, -1.0f}
  };
  
  size /= 2.0f;
  vector<vec3> positions(36);
  
  for (int i = 0; i < 6; ++i)
    positions[i] = vec3(seq[i].x, seq[i].y, -1.0f) * size;
  for (int i = 6; i < 12; ++i)
    positions[i] = vec3(seq[i].x, seq[i].y, 1.0f) * size;
  
  for (int i = 0; i < 6; ++i)
    positions[i+12] = vec3(seq[i].x, 1.0f, seq[i].y) * size;
  for (int i = 6; i < 12; ++i)
    positions[i+12] = vec3(seq[i].x, -1.0f, seq[i].y) * size;
  
  for (int i = 0; i < 6; ++i)
    positions[i+24] = vec3(-1.0f, seq[i].x, seq[i].y) * size;
  for (int i = 6; i < 12; ++i)
    positions[i+24] = vec3(1.0f, seq[i].x, seq[i].y) * size;
  
  vector<vec3> normals(6, vec3(0.0f, 0.0f, -1.0f));
  normals.resize(12, vec3(0.0f, 0.0f, 1.0f));
  normals.resize(18, vec3(0.0f, 1.0f, 0.0f));
  normals.resize(24, vec3(0.0f, -1.0f, 0.0f));
  normals.resize(30, vec3(-1.0f, 0.0f, 0.0f));
  normals.resize(36, vec3(1.0f, 0.0f, 0.0f));
  
  meshData["position"].assign(&positions[0].x, &positions[0].x+positions.size()*3);
  meshData["normal"].assign(&normals[0].x, &normals[0].x+normals.size()*3);
  meshData.totalVertices = (int)positions.size();
  meshData.addSubMesh(0, (int)positions.size());
  
//  VertexBuffer &pos = meshData.buffers["position"];
//  VertexBuffer &nor = meshData.buffers["normal"];
  
  
  VertexMeshPtr mesh = Graphics::getInstance().createVertexMesh();
  mesh->load(meshData);
  return mesh;
}
