#include "VulkanIncludes.hpp"


using namespace std;


VulkanImage::VulkanImage(VulkanDevice* device):
  mDevice(device),
  mVkImage(VK_NULL_HANDLE),
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
  imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.usage = mVmaMemoryUsage;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = mVmaMemoryUsage;
  allocInfo.flags = mVmaCreateFlags;
  allocInfo.priority = 1.0f;

  return vmaCreateImage(allocator, &imageInfo, &allocInfo, &mVkImage, &mVmaAllocation, nullptr) == VK_SUCCESS;
}

void VulkanImage::destroy() {
  if (mVkImage != VK_NULL_HANDLE) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaDestroyImage(allocator, mVkImage, mVmaAllocation);
    mVkImage = VK_NULL_HANDLE;
  }
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
