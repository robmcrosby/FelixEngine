//
//  VulkanCommandPool.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/11/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"


using namespace std;
using namespace fx;


VulkanCommandPool::VulkanCommandPool() {
  
}

VulkanCommandPool::~VulkanCommandPool() {
  clearCommandBuffers();
}

void VulkanCommandPool::submitRenderPasses(vector<RenderPass> &passes) {
  resizeCommandBuffers(passes.size());
  for (int i = 0; i < passes.size(); ++i)
    _commandBuffers.at(i)->submitRenderPass(passes.at(i));
}

void VulkanCommandPool::resizeCommandBuffers(size_t size) {
  while(_commandBuffers.size() < size)
    _commandBuffers.push_back(new VulkanCommandBuffer());
}

void VulkanCommandPool::clearCommandBuffers() {
  for (auto &buffer : _commandBuffers)
    delete buffer;
  _commandBuffers.clear();
}
