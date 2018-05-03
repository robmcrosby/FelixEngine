//
//  VulkanTextureBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanTextureBuffer.h"


using namespace std;
using namespace fx;

VulkanTextureBuffer::VulkanTextureBuffer() {
  
}

VulkanTextureBuffer::~VulkanTextureBuffer() {
  
}

bool VulkanTextureBuffer::load(const ImageBufferData &data) {
  return true;
}

bool VulkanTextureBuffer::loaded() const {
  return true;
}

ivec2 VulkanTextureBuffer::size() const {
  return ivec2();
}

void VulkanTextureBuffer::setDefaultSampler(SamplerState state) {
  
}

SamplerState VulkanTextureBuffer::defaultSampler() const {
  return SamplerState();
}
