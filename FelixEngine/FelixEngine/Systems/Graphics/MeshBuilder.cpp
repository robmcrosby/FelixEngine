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
