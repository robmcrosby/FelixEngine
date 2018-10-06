//
//  RenderPass.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 10/6/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "RenderPass.h"
#include "Scene.h"


using namespace fx;
using namespace std;


RenderPass::RenderPass(): _scene(0), sortItems(0), itemsSorted(0) {
  
}

RenderPass::~RenderPass() {
  
}

void RenderPass::addRenderItem(const RenderItem &item) {
  itemsSorted = false;
  renderItems.push_back(item);
}

void RenderPass::clearRenderItems() {
  itemsSorted = true;
  renderItems.clear();
}

FrameBufferPtr RenderPass::getFrame() {
  if (!frame)
    frame = Graphics::getInstance().createFrameBuffer();
  return frame;
}

ivec2 RenderPass::getFrameSize() {
  return getFrame()->size();
}

void RenderPass::setFrame(const std::string &name) {
  if (_scene != nullptr) {
    frame = _scene->getFrameBuffer(name);
  }
  else {
    frame = Graphics::getInstance().getFrameBuffer(name);
  }
}

bool RenderPass::setFrame(const XMLTree::Node &node) {
  if (node.hasAttribute("name"))
    setFrame(node.attribute("name"));
  return getFrame()->loadXML(node);
}

bool RenderPass::setFrameToWindow(int index) {
  return getFrame()->setToWindow(index);
}

bool RenderPass::resizeFrame(int width, int height) {
  return getFrame()->resize(width, height);
}

bool RenderPass::addColorTexture() {
  return getFrame()->addColorTexture();
}

bool RenderPass::addDepthBuffer() {
  return getFrame()->addDepthBuffer();
}

TextureBufferPtr RenderPass::getColorTexture(int index) {
  return getFrame()->getColorTexture(0);
}

bool RenderPass::setClearSettings(const XMLTree::Node &node) {
  if (node.hasAttribute("color"))
    setClearColor(node.attribute("color"));
  if (node.hasAttribute("depth"))
    setClearDepthStencil(node.attributeAsFloat("depth"));
  return true;
}

void RenderPass::setClearColor(const vec4 &color) {
  for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
    colorActions[i].loadAction = LOAD_CLEAR;
    colorActions[i].clearColor = color;
  }
}

void RenderPass::setDefaultColorActions() {
  for (int i = 0; i < MAX_COLOR_ATTACHEMENTS; ++i) {
    colorActions[i].loadAction = LOAD_LAST;
    colorActions[i].storeAction = STORE_SAVE;
    colorActions[i].clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  }
}

void RenderPass::setClearDepthStencil(float depth, int stencil) {
  depthStencilAction.loadAction = LOAD_CLEAR;
  depthStencilAction.clearColor = vec4(depth, (float)stencil, 0.0f, 1.0f);
}

void RenderPass::setDefaultDepthStencilActions() {
  depthStencilAction.loadAction = LOAD_LAST;
  depthStencilAction.storeAction = STORE_SAVE;
  depthStencilAction.clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void RenderPass::addUniformMap(UniformsPtr uniformMap) {
  uniforms.push_back(uniformMap);
}

UniformMap& RenderPass::getUniformMap(int index) {
  while (uniforms.size() <= index)
    uniforms.push_back(UniformMap::make());
  return *uniforms.at(index).get();
}

void RenderPass::clearUniformMaps() {
  uniforms.clear();
}

void RenderPass::setScene(Scene *scene) {
  _scene = scene;
}

bool RenderPass::loadXML(const XMLTree::Node &node) {
  bool success = true;
  if (node.hasAttribute("width") && node.hasAttribute("height"))
    resizeFrame(node.attributeAsInt("width"), node.attributeAsInt("height"));
  if (node.hasAttribute("frame"))
    setFrame(node.attribute("frame"));
  if (node.hasAttribute("sorted"))
    sortItems = node.attributeAsBoolean("sorted");
  for (auto subNode : node)
    success &= loadXMLItem(*subNode);
  return success;
}

bool RenderPass::loadXMLItem(const XMLTree::Node &node) {
  if (node == "Frame")
    return setFrame(node);
  if (node == "Clear")
    return setClearSettings(node);
  std::cerr << "Unknown XML Node in RenderPass:" << std::endl << node << std::endl;
  return false;
}

void RenderPass::sortRenderItems() {
  if (sortItems && !itemsSorted) {
    sort(renderItems.begin(), renderItems.end());
    itemsSorted = true;
  }
}
