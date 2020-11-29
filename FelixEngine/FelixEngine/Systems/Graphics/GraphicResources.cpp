//
//  GraphicResources.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 7/17/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "GraphicResources.h"
#include "FileSystem.h"
#include "MeshLoader.h"
#include "USDCrate.h"


using namespace fx;
using namespace std;


bool ShaderProgram::load(const XMLTree::Node &node) {
  if (node.hasAttributes("vertex", "fragment"))
    return loadShaderFunctions(node.attribute("vertex"), node.attribute("fragment"));
  return false;
}


bool VertexMesh::load(const XMLTree::Node &node) {
  VertexMeshData meshData;
  if (node.hasAttribute("file")) {
    if (FileSystem::loadMesh(meshData, node.attribute("file")))
      return load(meshData);
  }
  else {
    if (meshData.load(node))
      return load(meshData);
  }
  return false;
}

bool VertexMesh::load(const USDItem &item) {
  VertexMeshData data;
  return MeshLoader::loadFromCrateFile(data, item) && load(data);
}

bool VertexMesh::setVertexBuffer(const string &name, const vector<float> &buffer) {
  return setVertexBuffer(name, 1, buffer.size(), &buffer.at(0));
}

bool VertexMesh::setVertexBuffer(const string &name, const vector<vec2> &buffer) {
  return setVertexBuffer(name, 2, buffer.size(), &buffer.at(0).x);
}

bool VertexMesh::setVertexBuffer(const string &name, const vector<vec4> &buffer) {
  return setVertexBuffer(name, 4, buffer.size(), &buffer.at(0).x);
}


bool TextureBuffer::load(const XMLTree::Node &node) {
  setDefaultSampler(node);
  if (node.hasAttribute("file"))
    return loadImageFile(node.attribute("file"), defaultSampler().mipMappingEnabled());
  return false;
}

bool TextureBuffer::loadImageFile(const string &file, bool generateMipMap) {
  ImageBufferData image;
  if (FileSystem::loadImage(image, file))
    return loadImage(image, generateMipMap);
  return false;
}

bool TextureBuffer::loadCubeMapFile(const string &file, bool generateMipMap) {
  ImageBufferData image;
  if (FileSystem::loadImage(image, file))
    return loadCubeMap(image, generateMipMap);
  return false;
}

bool TextureBuffer::loadCubeMapFiles(const vector<string> &files, bool generateMipMap) {
  ImageBufferSet images(files.size());
  for (int i = 0; i < files.size(); ++i) {
    if (!FileSystem::loadImage(images[i], files[i]))
      return false;
  }
  return loadCubeMap(images, generateMipMap);
}

bool TextureBuffer::loadColor(const RGBA &color) {
  ImageBufferData image;
  image.resize(1, 1);
  image[0] = color;
  return loadImage(image, false);
}


bool FrameBuffer::load(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("window"))
    success &= setToWindow(node.attributeAsInt("window"));
  
  for (auto subnode : node) {
    if (subnode->attribute("type") == "depth")
      addDepthBuffer();
    else
      addColorTexture();
  }
  return success;
}
