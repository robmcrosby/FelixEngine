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
  _textureFormat      = VK_FORMAT_UNDEFINED;
  _textureImage       = VK_NULL_HANDLE;
  _textureImageMemory = VK_NULL_HANDLE;
  _textureImageView   = VK_NULL_HANDLE;
}

VulkanTextureBuffer::~VulkanTextureBuffer() {
  clearTexture();
}

bool VulkanTextureBuffer::loadImage(const ImageBufferData &image, bool generateMipMap) {
  size_t imageSize = image.sizeInBytes();
  _width = image.size.w;
  _height = image.size.h;
  
  // Set the texture format
  _textureFormat = VK_FORMAT_R8G8B8A8_UNORM;
  
  // Create and Allocate the Staging Buffer
  VkBuffer stagingBuffer = Vulkan::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  VkDeviceMemory stagingBufferMemory = Vulkan::allocateMemory(stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  // Upload the Image to the Staging Buffer
  Vulkan::upload(stagingBufferMemory, image.ptr(), imageSize);
  
  // Create and Allocate the Texture Image
  _textureImage = Vulkan::createImage(_width, _height, _textureFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
  _textureImageMemory = Vulkan::allocateImage(_textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  
  // Transfer the Texture from the Buffer to Image
  Vulkan::transitionImageLayout(_textureImage, _textureFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  Vulkan::copyBufferToImage(stagingBuffer, _textureImage, _width, _height);
  Vulkan::transitionImageLayout(_textureImage, _textureFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  
  // Create the Texture Image View
  _textureImageView = Vulkan::createImageView(_textureImage, _textureFormat, VK_IMAGE_ASPECT_COLOR_BIT);
  
  // Free the Staging Buffer
  vkDestroyBuffer(Vulkan::device, stagingBuffer, nullptr);
  vkFreeMemory(Vulkan::device, stagingBufferMemory, nullptr);
  
  return true;
}

bool VulkanTextureBuffer::loadCubeMap(const ImageBufferSet &images) {
  return false;
}

bool VulkanTextureBuffer::loadSwapBuffer(VkFormat format, VkImage image, int width, int height) {
  _width = width;
  _height = height;
  
  _textureFormat = format;
  _textureImage = image;
  
  VkImageViewCreateInfo imageViewInfo = {};
  imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewInfo.pNext = NULL;
  imageViewInfo.format = format;
  imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
  imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
  imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
  imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
  imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageViewInfo.subresourceRange.baseMipLevel = 0;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.layerCount = 1;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewInfo.flags = 0;
  imageViewInfo.image = image;
  
  bool success = vkCreateImageView(Vulkan::device, &imageViewInfo, NULL, &_textureImageView) == VK_SUCCESS;
  if (!success) {
    cerr << "Error creating Vulkan Image View for Swap Buffer" << endl;
    assert(false);
  }
  return success;
}

bool VulkanTextureBuffer::loadDepthBuffer(VkFormat format, int width, int height) {
  _width = width;
  _height = height;
  
  _textureFormat = format;
  _textureImage = Vulkan::createImage(_width, _height, _textureFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
  _textureImageMemory = Vulkan::allocateImage(_textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  _textureImageView = Vulkan::createImageView(_textureImage, _textureFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
  
  Vulkan::transitionImageLayout(_textureImage, _textureFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  
  return _textureImageView != VK_NULL_HANDLE;
}

bool VulkanTextureBuffer::loaded() const {
  return true;
}

ivec2 VulkanTextureBuffer::size() const {
  return ivec2(_width, _height);
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
