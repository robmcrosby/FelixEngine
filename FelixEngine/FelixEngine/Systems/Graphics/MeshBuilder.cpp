//
//  MeshBuilder.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 3/14/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "MeshBuilder.h"


using namespace fx;
using namespace std;

VertexMeshPtr MeshBuilder::loadFromFile(const std::string &file) {
  return Graphics::getInstance().createVertexMesh();
}
