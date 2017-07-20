//
//  GraphicResources.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/17/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "FileSystem.h"


using namespace fx;
using namespace std;


bool ShaderProgram::loadXML(const XMLTree::Node &node) {
  if (node.hasAttributes("vertexFunction", "fragmentFunction"))
    return loadShaderFunctions(node.attribute("vertexFunction"), node.attribute("fragmentFunction"));
  return false;
}


bool VertexMesh::loadXML(const XMLTree::Node &node) {
  if (node.hasAttribute("file")) {
    fx::VertexMeshData meshData;
    if (fx::FileSystem::loadMesh(meshData, node.attribute("file")))
      return load(meshData);
  }
  return false;
}
