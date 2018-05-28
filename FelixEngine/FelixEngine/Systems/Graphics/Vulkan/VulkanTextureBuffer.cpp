//
//  VulkanTextureBuffer.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 5/3/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#include "VulkanTextureBuffer.h"
#include "Vulkan.h"
#include "ImageBufferData.h"


using namespace std;
using namespace fx;

map<int, VkSampler> VulkanTextureBuffer::samplerMap;


VulkanTextureBuffer::VulkanTextureBuffer() {
  _textureImage       = VK_NULL_HANDLE;
  _textureImageMemory = VK_NULL_HANDLE;
  _textureImageView   = VK_NULL_HANDLE;
}

VulkanTextureBuffer::~VulkanTextureBuffer() {
  clearTexture();
}

bool VulkanTextureBuffer::load(const ImageBufferData &data) {
  size_t imageSize = data.sizeInBytes();
  size_t width = (size_t)data.size.w;
  size_t height = (size_t)data.size.h;
  
  // Create and Allocate the Staging Buffer
  VkBuffer stagingBuffer = Vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  VkDeviceMemory stagingBufferMemory = Vulkan::allocateMemory(stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  // Upload the Image to the Staging Buffer
  Vulkan::upload(stagingBufferMemory, data.ptr(), imageSize);
  
  // Create and Allocate the Texture Image
  _textureImage = Vulkan::createImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
  _textureImageMemory = Vulkan::allocateImage(_textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  
  // Transfer the Texture from the Buffer to Image
  Vulkan::transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  Vulkan::copyBufferToImage(stagingBuffer, _textureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
  Vulkan::transitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  
  // Create the Texture Image View
  _textureImageView = Vulkan::createImageView(_textureImage, VK_FORMAT_R8G8B8A8_UNORM);
  
  // Free the Staging Buffer
  vkDestroyBuffer(Vulkan::device, stagingBuffer, nullptr);
  vkFreeMemory(Vulkan::device, stagingBufferMemory, nullptr);
  
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

void VulkanTextureBuffer::clearTexture() {
  if (_textureImageView != VK_NULL_HANDLE) {
    vkDestroyImageView(Vulkan::device, _textureImageView, nullptr);
    _textureImageView = VK_NULL_HANDLE;
  }
  if (_textureImage != VK_NULL_HANDLE) {
    vkDestroyImage(Vulkan::device, _textureImage, nullptr);
    _textureImage = VK_NULL_HANDLE;
  }
  if (_textureImageMemory != VK_NULL_HANDLE) {
    vkFreeMemory(Vulkan::device, _textureImageMemory, nullptr);
    _textureImageMemory = VK_NULL_HANDLE;
  }
}

VkSampler VulkanTextureBuffer::getSamplerForState(const SamplerState &state) {
  int flags = state.flags;
  if (samplerMap.count(flags) == 0) {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    
    samplerInfo.magFilter = state.magFilter() == FILTER_LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    samplerInfo.minFilter = state.minFilter() == FILTER_LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    
    samplerInfo.addressModeU = getAddressMode(state.sCoord());
    samplerInfo.addressModeV = getAddressMode(state.tCoord());
    samplerInfo.addressModeW = getAddressMode(state.rCoord());
    
    samplerInfo.anisotropyEnable = state.samples() > 1 ? VK_TRUE : VK_FALSE;
    samplerInfo.maxAnisotropy = (float)state.samples();
    
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    
    VkSampler sampler = VK_NULL_HANDLE;
    if (vkCreateSampler(Vulkan::device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
      cerr << "Error creating Vulkan Texture Sampler" << endl;
      assert(false);
    }
    samplerMap[flags] = sampler;
  }
  return samplerMap.at(flags);
}

VkSamplerAddressMode VulkanTextureBuffer::getAddressMode(COORD_TYPE type) {
  switch (type) {
    case COORD_CLAMP_EDGE:
      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case COORD_REPEAT:
      return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case COORD_MIRROR:
      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case COORD_CLAMP_ZERO:
      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
  }
}
