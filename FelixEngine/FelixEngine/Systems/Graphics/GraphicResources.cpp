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
#include "Quaternion.h"
#include "RenderPass.h"


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

bool TextureBuffer::loadCubeColor(const RGBA &color) {
  vector<ImageBufferData> images(6);
  for (auto &image : images) {
    image.resize(1, 1);
    image[0] = color;
  }
  return loadCubeMap(images, false);
}

void TextureBuffer::generateMipMap() {
  TextureBufferPtr thisTexture = shared_from_this();
  CommandBufferPtr commandBuffer = Graphics::getInstance().createCommandBuffer();
  commandBuffer->encodeGenerateMipmap(thisTexture);
  commandBuffer->commit();
}

void TextureBuffer::generateCubeMap(const ImageBufferData &srcImage, bool mipmapped) {
  float buffer[] = {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f};
  
  // Rotations
  vector<quat> rotationData;
  rotationData.push_back(quat::RotZ(M_PI/2.0f));  // Right
  rotationData.push_back(quat::RotZ(-M_PI/2.0f)); // Left
  rotationData.push_back(quat::RotY(-M_PI/2.0f)); // Up
  rotationData.push_back(quat::RotY(M_PI/2.0f));  // Down
  rotationData.push_back(quat::RotZ(0.0f));       // Front
  rotationData.push_back(quat::RotZ(M_PI));       // Back
  
  SamplerState sampler;
  sampler.setSCoord(COORD_REPEAT);
  sampler.setTCoord(COORD_REPEAT);
  sampler.setMinFilter(FILTER_LINEAR);
  sampler.setMagFilter(FILTER_LINEAR);
  
  RenderItem renderItem;
  renderItem.loadShaderFunctions("v_cube_gen", "f_cube_gen");
  renderItem.setMeshVertexBuffer("vertices", 2, 4, buffer);
  renderItem.setMeshPrimativeType(VERTEX_TRIANGLE_STRIP);
  renderItem.setTexture("srcTexture", srcImage, sampler);
  
  RenderPassPtr renderPass = Graphics::getInstance().createRenderPass();
  renderPass->addRenderItem(renderItem);
  renderPass->resizeFrame(size().width, size().height);
  TextureBufferPtr target = renderPass->addRenderTarget(format(), TEXTURE_SHADER_READ);

  TextureBufferPtr thisTexture = shared_from_this();
  CommandBufferPtr commandBuffer = Graphics::getInstance().createCommandBuffer();
  for (int i = 0; i < 6; ++i) {
    renderPass->getUniformMap()["rotation"] = rotationData[i];
    commandBuffer->encodeRenderPass(renderPass);
    commandBuffer->encodeBlitTexture(target, thisTexture, i);
  }
  if (mipmapped) {
    commandBuffer->encodeGenerateMipmap(thisTexture);
  }
  commandBuffer->commit();
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

TextureBufferPtr FrameBuffer::addRenderTarget(TEXTURE_FORMAT format, int usageFlags) {
  TextureBufferPtr texture = Graphics::getInstance().createTextureBuffer();
  texture->loadBuffer(size(), format, usageFlags | TEXTURE_RENDER_TARGET);
  addRenderTarget(texture);
  return texture;
}

TextureBufferPtr FrameBuffer::addColorTexture() {
  return addRenderTarget(TEXTURE_RGBA8, TEXTURE_SHADER_READ);
}

TextureBufferPtr FrameBuffer::addDepthBuffer() {
  return addRenderTarget(TEXTURE_DEPTH32F);
}
