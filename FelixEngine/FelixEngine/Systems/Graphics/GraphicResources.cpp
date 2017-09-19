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
  VertexMeshData meshData;
  
  if (node.hasAttribute("file")) {
    if (fx::FileSystem::loadMesh(meshData, node.attribute("file")))
      return load(meshData);
  }
  else {
    if (meshData.loadXML(node))
      return load(meshData);
  }
  return false;
}

bool FrameBuffer::loadXML(const XMLTree::Node &node) {
  bool success = true;
  //if (node.hasAttribute("window"))
  //  success &= setToWindow(node.attributeAsInt("window"));
  
  for (auto subnode : node) {
    if (subnode->attribute("type") == "depth")
      addDepthBuffer();
    else
      addColorTexture();
  }
  return success;
}
