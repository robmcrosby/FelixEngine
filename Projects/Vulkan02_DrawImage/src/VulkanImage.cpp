#include "VulkanIncludes.hpp"


using namespace std;


VulkanImage::VulkanImage(VulkanDevice* device):
  mDevice(device),
  mVkFormat(VK_FORMAT_R8G8B8A8_UNORM),
  mVkImage(VK_NULL_HANDLE),
  mVkImageView(VK_NULL_HANDLE),
  mVkImageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
  mVkImageUsageFlags(0),
  mVmaMemoryUsage(VMA_MEMORY_USAGE_AUTO),
  mVmaCreateFlags(0) {

}

VulkanImage::~VulkanImage() {
  destroy();
}

void VulkanImage::setUsage(VkImageUsageFlags flags) {
  if (mVkImage == VK_NULL_HANDLE)
    mVkImageUsageFlags = flags;
  else
    cerr << "Warning: VulkanImage usage flags must be set before allocation" << endl;
}

void VulkanImage::setCreateFlags(VmaAllocationCreateFlags flags) {
  if (mVkImage == VK_NULL_HANDLE)
    mVmaCreateFlags = flags;
  else
    cerr << "Warning: VmaCreate flags must be set before allocation" << endl;
}

bool VulkanImage::alloc(uint32_t width, uint32_t height) {
  VmaAllocator allocator = mDevice->getVmaAllocator();

  VkImageCreateInfo imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, 0, 0};
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = mVkFormat;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage = mVkImageUsageFlags;
  imageInfo.initialLayout = mVkImageLayout;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = mVmaMemoryUsage;
  allocInfo.flags = mVmaCreateFlags;
  allocInfo.priority = 1.0f;

  return vmaCreateImage(allocator, &imageInfo, &allocInfo, &mVkImage, &mVmaAllocation, nullptr) == VK_SUCCESS;
}

void VulkanImage::destroy() {
  if (mVkImageView != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyImageView(device, mVkImageView, nullptr);
    mVkImageView = VK_NULL_HANDLE;
  }
  if (mVkImage != VK_NULL_HANDLE) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaDestroyImage(allocator, mVkImage, mVmaAllocation);
    mVkImage = VK_NULL_HANDLE;
  }
}

VkImageView VulkanImage::getVkImageView() {
  if (mVkImageView == VK_NULL_HANDLE && mVkImage != VK_NULL_HANDLE)
    mVkImageView = createImageView(mVkImage, mVkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
  return mVkImageView;
}

VmaAllocationInfo VulkanImage::getVmaAllocationInfo() const {
  VmaAllocationInfo info;
  if (mVkImage != VK_NULL_HANDLE) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaGetAllocationInfo(allocator, mVmaAllocation, &info);
  }
  else {
    cerr << "Error: VmaAllocationInfo not avalible for empty VulkanBuffer" << endl;
  }
  return info;
}

void* VulkanImage::data() {
  VmaAllocationInfo info = getVmaAllocationInfo();
  return info.pMappedData;
}

void VulkanImage::transitionToLayout(VkCommandBuffer commandBuffer, VkImageLayout layout) {
  VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
  barrier.oldLayout = mVkImageLayout;
  barrier.newLayout = layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = mVkImage;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags srcStage = 0;
  VkPipelineStageFlags dstStage = 0;
  if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
    srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
  }

  vkCmdPipelineBarrier(
    commandBuffer,
    srcStage, dstStage,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier
  );

  mVkImageLayout = layout;
}

VkImageView VulkanImage::createImageView(
  VkImage image,
  VkFormat format,
  VkImageAspectFlags aspectFlags,
  int32_t mipLevels
) const {
  VkImageViewCreateInfo viewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView = VK_NULL_HANDLE;
  VkDevice device = mDevice->getVkDevice();
  if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    cerr << "Error creating Image View" << endl;
  return imageView;
}
