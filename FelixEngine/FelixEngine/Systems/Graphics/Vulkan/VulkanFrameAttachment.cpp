//
//  VulkanFrameAttachment.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/8/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanFrameAttachment.h"


using namespace std;
using namespace fx;

VulkanFrameAttachment::VulkanFrameAttachment() {
  _isSwapChain = false;
}

VulkanFrameAttachment::~VulkanFrameAttachment() {
  clearBuffers();
}

bool VulkanFrameAttachment::loadSwapBuffers(int index) {
  _textureBuffers = Vulkan::swapChainBuffers;
  _isSwapChain = true;
  return true;
}

bool VulkanFrameAttachment::loadDepthBuffer(VkFormat format, int width, int height) {
  shared_ptr<VulkanTextureBuffer> depthTexture = make_shared<VulkanTextureBuffer>();
  if (depthTexture->loadDepthBuffer(format, width, height)) {
    _textureBuffers.push_back(depthTexture);
    return true;
  }
  return false;
}

VkFormat VulkanFrameAttachment::getFormat() const {
  if (_textureBuffers.size() > 0)
    return _textureBuffers.front()->getFormat();
  return VK_FORMAT_UNDEFINED;
}

VkImageView VulkanFrameAttachment::getImageView(int index) const {
  if (index < _textureBuffers.size())
    return _textureBuffers.at(index)->getImageView();
  if (_textureBuffers.size() > 0)
    return _textureBuffers.front()->getImageView();
  return VK_NULL_HANDLE;
}

ivec2 VulkanFrameAttachment::getBufferSize() const {
  if (_textureBuffers.size() > 0)
    return _textureBuffers.front()->size();
  return ivec2(0, 0);
}

int VulkanFrameAttachment::getCurrentIndex() const {
  if (_isSwapChain)
    return Vulkan::currentSwapBuffer;
  return 0;
}

void VulkanFrameAttachment::clearBuffers() {
  _frameBuffers.clear();
  _textureBuffers.clear();
}
