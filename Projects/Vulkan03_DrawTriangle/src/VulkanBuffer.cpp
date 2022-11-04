#include "VulkanIncludes.hpp"


using namespace std;


VulkanBuffer::VulkanBuffer(VulkanDevice* device):
  mDevice(device),
  mVkBuffer(VK_NULL_HANDLE),
  mVkBufferUsageFlags(0),
  mVmaMemoryUsage(VMA_MEMORY_USAGE_AUTO),
  mVmaCreateFlags(0),
  mSize(0) {

}

VulkanBuffer::~VulkanBuffer() {
  destroy();
}

void VulkanBuffer::setUsage(VkBufferUsageFlags flags) {
  if (mVkBuffer == VK_NULL_HANDLE)
    mVkBufferUsageFlags = flags;
  else
    cerr << "Warning: VulkanBuffer usage flags must be set before allocation" << endl;
}

void VulkanBuffer::setCreateFlags(VmaAllocationCreateFlags flags) {
  if (mVkBuffer == VK_NULL_HANDLE)
    mVmaCreateFlags = flags;
  else
    cerr << "Warning: VmaCreate flags must be set before allocation" << endl;
}

bool VulkanBuffer::alloc(VkDeviceSize size) {
  VmaAllocator allocator = mDevice->getVmaAllocator();

  VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  bufferInfo.size = size;
  bufferInfo.usage = mVkBufferUsageFlags;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = mVmaMemoryUsage;
  allocInfo.flags = mVmaCreateFlags;

  mSize = size;
  VmaAllocationInfo info;
  return vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &mVkBuffer, &mVmaAllocation, &info) == VK_SUCCESS;
}

void VulkanBuffer::destroy() {
  if (mVkBuffer != VK_NULL_HANDLE) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaDestroyBuffer(allocator, mVkBuffer, mVmaAllocation);
    mVkBuffer = VK_NULL_HANDLE;
  }
}

void VulkanBuffer::copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer) {
  auto dstBuffer = buffer->getVkBuffer();
  if (dstBuffer != VK_NULL_HANDLE && mVkBuffer != VK_NULL_HANDLE) {
    VkBufferCopy bufferCopy = {0, 0, mSize};
    vkCmdCopyBuffer(commandBuffer, mVkBuffer, dstBuffer, 1, &bufferCopy);
  }
}

VmaAllocationInfo VulkanBuffer::getVmaAllocationInfo() const {
  VmaAllocationInfo info;
  if (mVkBuffer != VK_NULL_HANDLE) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaGetAllocationInfo(allocator, mVmaAllocation, &info);
  }
  else
    cerr << "Error: VmaAllocationInfo not avalible for empty VulkanBuffer" << endl;
  return info;
}

VkMemoryPropertyFlags VulkanBuffer::getVkMemoryPropertyFlags() const {
  VkMemoryPropertyFlags flags = 0;
  if (mVkBuffer != VK_NULL_HANDLE) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaGetAllocationMemoryProperties(allocator, mVmaAllocation, &flags);
  }
  return flags;
}

bool VulkanBuffer::isHostVisible() const {
  return getVkMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

void* VulkanBuffer::data() {
  VmaAllocationInfo info = getVmaAllocationInfo();
  return info.pMappedData;
}
