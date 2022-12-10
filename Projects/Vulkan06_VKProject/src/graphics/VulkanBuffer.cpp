#include <VulkanIncludes.hpp>
#include <cstring>


using namespace std;


VulkanBuffer::VulkanBuffer(VulkanDevice* device):
  mDevice(device),
  mVkBufferUsageFlags(0),
  mVmaMemoryUsage(VMA_MEMORY_USAGE_AUTO),
  mVmaCreateFlags(0),
  mSize(0) {

}

VulkanBuffer::~VulkanBuffer() {
  destroy();
}

void VulkanBuffer::setUsage(VkBufferUsageFlags flags) {
  if (mVkBuffers.empty())
    mVkBufferUsageFlags = flags;
  else
    cerr << "Warning: VulkanBuffer usage flags must be set before allocation" << endl;
}

void VulkanBuffer::setCreateFlags(VmaAllocationCreateFlags flags) {
  if (mVkBuffers.empty())
    mVmaCreateFlags = flags;
  else
    cerr << "Warning: VmaCreate flags must be set before allocation" << endl;
}

bool VulkanBuffer::alloc(VkDeviceSize size, int frames) {
  VmaAllocator allocator = mDevice->getVmaAllocator();

  VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
  bufferInfo.size = size;
  bufferInfo.usage = mVkBufferUsageFlags;

  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = mVmaMemoryUsage;
  allocInfo.flags = mVmaCreateFlags;

  mSize = size;
  VmaAllocationInfo info;

  clearBuffers();
  for (int i = 0; i < frames; ++i) {
    VkBuffer buffer;
    VmaAllocation allocation;
    if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, &info) != VK_SUCCESS) {
      cerr << "Error allocating VulkanBuffer!" << endl;
      clearBuffers();
      return false;
    }
    mVkBuffers.push_back(buffer);
    mVmaAllocations.push_back(allocation);

    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range  = VK_WHOLE_SIZE;
    mVkDescriptorBufferInfos.push_back(bufferInfo);
  }
  return true;
}

void VulkanBuffer::destroy() {
  clearBuffers();
}

void VulkanBuffer::clearBuffers() {
  VmaAllocator allocator = mDevice->getVmaAllocator();
  for (int i = 0; i < mVkBuffers.size(); ++i) {
    vmaDestroyBuffer(allocator, mVkBuffers.at(i), mVmaAllocations.at(i));
  }

  mVkBuffers.clear();
  mVmaAllocations.clear();
  mVkDescriptorBufferInfos.clear();
}

void VulkanBuffer::copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame) {
  copyToBuffer(commandBuffer, buffer->getVkBuffer(), frame);
}

void VulkanBuffer::copyToBuffer(VkCommandBuffer commandBuffer, VkBuffer dst, int frame) {
  if (dst != VK_NULL_HANDLE && !mVkBuffers.empty()) {
    VkBufferCopy bufferCopy = {0, 0, mSize};
    vkCmdCopyBuffer(commandBuffer, mVkBuffers.at(frame), dst, 1, &bufferCopy);
  }
}

VmaAllocationInfo VulkanBuffer::getVmaAllocationInfo(int frame) const {
  VmaAllocationInfo info;
  if (!mVmaAllocations.empty()) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaGetAllocationInfo(allocator, mVmaAllocations.at(frame), &info);
  }
  else
    throw runtime_error("Error getting VmaAllocationInfo, empty VulkanBuffer");
  return info;
}

VkMemoryPropertyFlags VulkanBuffer::getVkMemoryPropertyFlags(int frame) const {
  VkMemoryPropertyFlags flags = 0;
  if (!mVmaAllocations.empty()) {
    VmaAllocator allocator = mDevice->getVmaAllocator();
    vmaGetAllocationMemoryProperties(allocator, mVmaAllocations.at(frame), &flags);
  }
  else
    throw runtime_error("Error getting VkMemoryPropertyFlags, empty VulkanBuffer");
  return flags;
}

VkDescriptorType VulkanBuffer::getVkDescriptorType() const {
  if (mVkBufferUsageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
}

bool VulkanBuffer::isHostVisible(int frame) const {
  return getVkMemoryPropertyFlags(frame) & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

void* VulkanBuffer::data(int frame) {
  VmaAllocationInfo info = getVmaAllocationInfo(frame);
  return info.pMappedData;
}

bool VulkanBuffer::load(const void* data, VkDeviceSize size, int frames) {
  // Determine the number of Frames
  frames = frames > 0 ? frames : mVkBuffers.empty() ? 1 : (int)mVkBuffers.size();

  if (mSize < size || frames != mVkBuffers.size()) {
    clearBuffers();
    if (!alloc(size, frames))
      return false;
  }

  if (isHostVisible()) {
    // Copy directly to the buffers
    for (int frame = 0; frame < frames; frame++)
      update(data, size, frame);
    return true;
  }
  return false;
}

bool VulkanBuffer::load(VulkanQueuePtr queue, const void* data, VkDeviceSize size, int frames) {
  // Determine the number of Frames
  frames = frames > 0 ? frames : mVkBuffers.empty() ? 1 : (int)mVkBuffers.size();

  if (mSize < size || frames != mVkBuffers.size()) {
    clearBuffers();
    if (!alloc(size, frames))
      return false;
  }

  if (isHostVisible()) {
    // Copy directly to the buffers
    for (int frame = 0; frame < frames; frame++)
      update(data, size, frame);
    return true;
  }
  else {
    // Create staging buffer
    auto staging = mDevice->createBuffer();
    staging->setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    staging->setCreateFlags(
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
      VMA_ALLOCATION_CREATE_MAPPED_BIT
    );

    // Load staging buffer
    if (!staging->load(data, size))
      return false;

    // Copy staging buffer to each buffer
    if (auto command = queue->beginSingleCommand()) {
      for (int frame = 0; frame < frames; frame++)
        staging->copyToBuffer(command->getVkCommandBuffer(), getVkBuffer(frame));
      command->endSingle();
      queue->waitIdle();
      return true;
    }
  }
  return false;
}

void VulkanBuffer::update(const void* data, VkDeviceSize size, int frame) {
  size = min(mSize, size);
  memcpy(this->data(frame), data, size);
}
