#include <VulkanTypes.hpp>

#ifndef VulkanLayout_hpp
#define VulkanLayout_hpp


class VulkanLayout {
private:
  VulkanDevice*         mDevice;
  int                   mFrames;
  VulkanBufferMap       mBufferMap;
  BufferLayoutBindings  mBufferLayoutBindings;
  TextureLayoutBindings mTextureLayoutBindings;
  VkDescriptorSetLayout mVkDescriptorSetLayout;
  VkDescriptorPool      mVkDescriptorPool;
  VkDescriptorSets      mVkDescriptorSets;

public:
  VulkanLayout(VulkanDevice* device, int frames);
  ~VulkanLayout();

  bool setStorage(
    VulkanQueuePtr queue,
    uint32_t binding,
    const void* data,
    VkDeviceSize size,
    int frames = 0
  );
  bool setUniform(
    VulkanQueuePtr queue,
    uint32_t binding,
    const void* data,
    VkDeviceSize size,
    int frames = 0
  );

  template <typename T>
  bool setStorage(
    VulkanQueuePtr queue,
    uint32_t binding,
    const std::vector<T>& src,
    int frames = 0
  ) {
    return setStorage(queue, binding, src.data(), src.size() * sizeof(T), frames);
  }

  template <typename T>
  bool setUniform(
    VulkanQueuePtr queue,
    uint32_t binding,
    const std::vector<T>& src,
    int frames = 0
  ) {
    return setUniform(queue, binding, src.data(), src.size() * sizeof(T), frames);
  }

  bool setStorage(
    uint32_t binding,
    const void* data,
    VkDeviceSize size,
    int frames = 0
  );
  bool setUniform(
    uint32_t binding,
    const void* data,
    VkDeviceSize size,
    int frames = 0
  );

  template <typename T>
  bool setStorage(
    uint32_t binding,
    const std::vector<T>& src,
    int frames = 0
  ) {
    return setStorage(binding, src.data(), src.size() * sizeof(T), frames);
  }

  template <typename T>
  bool setStorage(
    uint32_t binding,
    const T& src,
    int frames = 0
  ) {
    return setStorage(binding, &src, sizeof(T), frames);
  }

  template <typename T>
  bool setUniform(
    uint32_t binding,
    const std::vector<T>& src,
    int frames = 0
  ) {
    return setUniform(binding, src.data(), src.size() * sizeof(T), frames);
  }

  template <typename T>
  bool setUniform(
    uint32_t binding,
    const T& src,
    int frames = 0
  ) {
    return setUniform(binding, &src, sizeof(T), frames);
  }

  void update(
    uint32_t binding,
    const void* data,
    VkDeviceSize size,
    int frame = 0
  );

  template <typename T>
  void update(
    uint32_t binding,
    const std::vector<T>& src,
    int frame = 0
  ) {
    update(binding, src.data(), src.size() * sizeof(T), frame);
  }

  template <typename T>
  void update(
    uint32_t binding,
    const T& src,
    int frame = 0
  ) {
    update(binding, &src, sizeof(T), frame);
  }

  void setBuffer(uint32_t binding, VulkanBufferPtr buffer);
  void setTexture(uint32_t binding, VulkanImagePtr image);
  void setTexture(uint32_t binding, VulkanImagePtr image, VulkanSamplerPtr sampler);
  void update();

  VkDescriptorSetLayout getVkDescriptorSetLayout();
  VkDescriptorPool      getVkDescriptorPool();
  VkDescriptorSet       getVkDescriptorSet(int frame);

  int frames() const {return mFrames;}

  void destroy();

private:
  void createDescriptorSets();
  void clearDescriptorSets();
  void destroyDescriptorPool();
  void destroyDescriptorSetLayout();
};


#endif /* VulkanLayout_hpp */
