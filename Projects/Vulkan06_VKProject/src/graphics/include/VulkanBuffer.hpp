#include <VulkanTypes.hpp>

#ifndef VulkanBuffer_hpp
#define VulkanBuffer_hpp


class VulkanBuffer {
private:
  VulkanDevice*  mDevice;
  VkBuffers      mVkBuffers;
  VmaAllocations mVmaAllocations;
  VkDeviceSize   mSize;

  VkBufferUsageFlags        mVkBufferUsageFlags;
  VmaMemoryUsage            mVmaMemoryUsage;
  VmaAllocationCreateFlags  mVmaCreateFlags;
  VkDescriptorBufferInfos   mVkDescriptorBufferInfos;

public:
  VulkanBuffer(VulkanDevice* device);
  ~VulkanBuffer();

  void setUsage(VkBufferUsageFlags flags);
  void setCreateFlags(VmaAllocationCreateFlags flags);

  bool alloc(VkDeviceSize size, int frames = 1);
  void destroy();
  void clearBuffers();

  void copyToBuffer(VkCommandBuffer commandBuffer, VulkanBufferPtr buffer, int frame = 0);
  void copyToBuffer(VkCommandBuffer commandBuffer, VkBuffer dst, int frame = 0);

  VkBuffer getVkBuffer(int frame = 0) const {
    return frame > frames() ? mVkBuffers.at(frame) : VK_NULL_HANDLE;
  }
  const VkBuffers& getVkBuffers() const {return mVkBuffers;}

  VmaAllocationInfo getVmaAllocationInfo(int frame = 0) const;
  VkMemoryPropertyFlags getVkMemoryPropertyFlags(int frame = 0) const;
  VkDescriptorType getVkDescriptorType() const;

  bool isHostVisible(int frame = 0) const;

  void* data(int frame = 0);
  VkDeviceSize size() const {return mSize;}
  uint32_t frames() const {return static_cast<uint32_t>(mVkBuffers.size());}
  bool allocated() const {return frames() > 0;}

  const VkDescriptorBufferInfo* getVkDescriptorBufferInfo(int index = 0) const {
    return &mVkDescriptorBufferInfos.at(index % frames());
  }

  bool load(const void* data, VkDeviceSize size, int frames = 0);
  bool load(VulkanQueuePtr queue, const void* data, VkDeviceSize size, int frames = 0);

  void update(const void* data, VkDeviceSize size, int frame = 0);

  template <typename T>
  bool load(VulkanQueuePtr queue, const std::vector<T>& src, int frames = 0) {
    return load(queue, src.data(), src.size() * sizeof(T), frames);
  }
};


#endif /* VulkanBuffer_hpp */
