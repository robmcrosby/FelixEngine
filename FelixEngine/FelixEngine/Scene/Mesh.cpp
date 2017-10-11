//
//  Mesh.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "Mesh.h"
#include "GraphicResources.h"

using namespace fx;
using namespace std;

MeshBuilder Mesh::meshBuilder = MeshBuilder();

Mesh::Mesh() {
  
}

Mesh::~Mesh() {
  
}

bool Mesh::loadXML(const XMLTree::Node &node) {
  _mesh = Graphics::getInstance().getVertexMesh(node.attribute("name"));
  return _mesh->loadXML(node);
}
