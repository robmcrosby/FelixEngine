#include "VulkanIncludes.hpp"


using namespace std;


VulkanImage::VulkanImage(VulkanDevice* device):
  mDevice(device),
  mVkFormat(VK_FORMAT_R8G8B8A8_UNORM),
  mVkImageUsageFlags(0),
  mVmaMemoryUsage(VMA_MEMORY_USAGE_AUTO),
  mVmaCreateFlags(0),
  mWidth(0),
  mHeight(0),
  mCurImageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
  mCurStageMask(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT),
  mCurAccessMask(0) {

}

VulkanImage::~VulkanImage() {
  destroy();
}

void VulkanImage::setUsage(VkImageUsageFlags flags) {
  if (!mVkImages.empty())
    mVkImageUsageFlags = flags;
  else
    cerr << "Warning: VulkanImage usage flags must be set before allocation" << endl;
}

void VulkanImage::setCreateFlags(VmaAllocationCreateFlags flags) {
  if (!mVkImages.empty())
    mVmaCreateFlags = flags;
  else
    cerr << "Warning: VmaCreate flags must be set before allocation" << endl;
}

bool VulkanImage::alloc(uint32_t width, uint32_t height, int frames) {
  mWidth = width;
  mHeight = height;
  for (int frame = 0; frame < frames; ++frame) {
    VkImage image;
    VmaAllocation allocation;
    if (!allocVkImage(image, allocation, width, height)) {
      cerr << "Error Allocating VulkanImage!" << endl;
      clearImages();
      return false;
    }
    mVkImages.push_back(image);
    mVmaAllocations.push_back(allocation);
    mVkImageViews.push_back(
      createImageView(image, mVkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    );
  }
  return mVkImages.size() == frames;
}

void VulkanImage::destroy() {
  clearImages();
}

void VulkanImage::clearImages() {
  // Destroy VkImageViews
  VkDevice device = mDevice->getVkDevice();
  for (auto imageView : mVkImageViews)
    vkDestroyImageView(device, imageView, nullptr);
  mVkImageViews.clear();

  // Destroy VkImages
  VmaAllocator allocator = mDevice->getVmaAllocator();
  for (int i = 0; i < mVmaAllocations.size(); ++i)
    vmaDestroyImage(allocator, mVkImages.at(i), mVmaAllocations.at(i));
  mVmaAllocations.clear();
  mVkImages.clear();

  mWidth = 0;
  mHeight = 0;
}

void VulkanImage::setSwapImages(const VkImages& images, VkFormat format, uint32_t width, uint32_t height) {
  clearImages();

  mVkFormat = format;
  mWidth = width;
  mHeight = height;

  for (auto image : images) {
    mVkImages.push_back(image);
    mVkImageViews.push_back(
      createImageView(image, mVkFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1)
    );
  }

  mCurImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  mCurAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  mCurStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
}

VmaAllocationInfo VulkanImage::getVmaAllocationInfo(int index) const {
  VmaAllocationInfo info;
  if (mVmaAllocations.size() > index) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaGetAllocationInfo(allocator, mVmaAllocations.at(index), &info);
  }
  else
    cerr << "Error: VmaAllocationInfo not avalible for empty VulkanBuffer" << endl;
  return info;
}

void* VulkanImage::data(int index) {
  VmaAllocationInfo info = getVmaAllocationInfo(index);
  return info.pMappedData;
}

VkDeviceSize VulkanImage::formatSize() const {
  if (mVkFormat == VK_FORMAT_R8G8B8A8_UNORM)
    return 4;
  return 1;
}

void VulkanImage::transition(
  VkCommandBuffer commandBuffer,
  VkImageLayout   newImageLayout,
  VkAccessFlags   dstAccessMask,
  VkPipelineStageFlags dstStageMask
) {
  mActiveBerriers.clear();
  for (auto image : mVkImages) {
    VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = mCurImageLayout;
    barrier.newLayout = newImageLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = mCurAccessMask;
    barrier.dstAccessMask = dstAccessMask;
    mActiveBerriers.push_back(barrier);
  }

  vkCmdPipelineBarrier(
    commandBuffer,
    mCurStageMask,
    dstStageMask,
    0,
    0, nullptr,
    0, nullptr,
    static_cast<uint32_t>(mActiveBerriers.size()),
    mActiveBerriers.data()
  );

  mCurImageLayout = newImageLayout;
  mCurStageMask = dstStageMask;
  mCurAccessMask = dstAccessMask;
}

void VulkanImage::copyFromBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame) {
  VkBuffer srcBuffer = buffer->getVkBuffer();
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {mWidth, mHeight, 1};

  vkCmdCopyBufferToImage(
    commandBuffer,
    srcBuffer,
    mVkImages.at(frame),
    mCurImageLayout,
    1,
    &region
  );
}

void VulkanImage::copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame) {
  VkBuffer dstBuffer = buffer->getVkBuffer();
  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {mWidth, mHeight, 1};

  vkCmdCopyImageToBuffer(
    commandBuffer,
    mVkImages.at(frame),
    mCurImageLayout,
    dstBuffer,
    1,
    &region
  );
}

bool VulkanImage::allocVkImage(
  VkImage&       image,
  VmaAllocation& allocation,
  uint32_t       width,
  uint32_t       height
) const {
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
  imageInfo.initialLayout = mCurImageLayout;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = mVmaMemoryUsage;
  allocInfo.flags = mVmaCreateFlags;
  allocInfo.priority = 1.0f;

  VmaAllocator allocator = mDevice->getVmaAllocator();
  return vmaCreateImage(allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr) == VK_SUCCESS;
}

VkImageView VulkanImage::createImageView(
  VkImage            image,
  VkFormat           format,
  VkImageAspectFlags aspectFlags,
  int32_t            mipLevels
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
