//
//  VulkanFrameBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanFrameBuffer.h"
#include "VulkanTextureBuffer.h"


using namespace std;
using namespace fx;

VulkanFrameBuffer::VulkanFrameBuffer() {
  
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
  
}

bool VulkanFrameBuffer::loadXML(const XMLTree::Node &node) {
  return true;
}

bool VulkanFrameBuffer::setToWindow(int index) {
  return true;
}

bool VulkanFrameBuffer::resize(int width, int height) {
  return true;
}

ivec2 VulkanFrameBuffer::size() const {
  return ivec2();
}

bool VulkanFrameBuffer::addDepthBuffer() {
  return true;
}

bool VulkanFrameBuffer::addColorTexture() {
  return true;
}

TextureBufferPtr VulkanFrameBuffer::getColorTexture(int index) {
  shared_ptr<VulkanTextureBuffer> texture = make_shared<VulkanTextureBuffer>();
  return texture;
}
