//
//  VulkanVertexMesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanVertexMesh.h"


using namespace std;
using namespace fx;

VulkanVertexMesh::VulkanVertexMesh() {
  
}

VulkanVertexMesh::~VulkanVertexMesh() {
  
}

bool VulkanVertexMesh::load(const VertexMeshData &data) {
  return true;
}

void VulkanVertexMesh::setPrimativeType(VERTEX_PRIMITIVE type) {
  
}

bool VulkanVertexMesh::setIndexBuffer(size_t count, const int *buffer) {
  return true;
}

bool VulkanVertexMesh::setVertexBuffer(const string &name, size_t size, size_t count, const float *buffer) {
  return true;
}
