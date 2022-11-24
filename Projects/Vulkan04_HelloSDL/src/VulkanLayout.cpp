#include "VulkanIncludes.hpp"
#include <map>


using namespace std;


VulkanLayout::VulkanLayout(VulkanDevice* device, int frames):
  mDevice(device),
  mFrames(frames),
  mVkDescriptorSetLayout(VK_NULL_HANDLE),
  mVkDescriptorPool(VK_NULL_HANDLE) {

}

VulkanLayout::~VulkanLayout() {
  destroy();
}

bool VulkanLayout::setStorage(
  VulkanQueuePtr queue,
  uint32_t binding,
  const void* data,
  VkDeviceSize size,
  int frames
) {
  frames = frames > 0 ? frames : mFrames;

  auto buffer = mDevice->createBuffer();
  buffer->setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
  );
  buffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
  );
  bool loaded = buffer->load(queue, data, size, frames);
  setBuffer(binding, buffer);
  return loaded;
}

bool VulkanLayout::setUniform(
  VulkanQueuePtr queue,
  uint32_t binding,
  const void* data,
  VkDeviceSize size,
  int frames
) {
  frames = frames > 0 ? frames : mFrames;

  auto buffer = mDevice->createBuffer();
  buffer->setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT |
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
  );
  buffer->setCreateFlags(VMA_ALLOCATION_CREATE_MAPPED_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
  );
  bool loaded = buffer->load(queue, data, size, frames);
  setBuffer(binding, buffer);
  return loaded;
}

void VulkanLayout::setBuffer(uint32_t binding, VulkanBufferPtr buffer) {
  BufferLayoutBinding layoutBinding;
  layoutBinding.buffer = buffer;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = buffer->getVkDescriptorType();
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_ALL_GRAPHICS;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  mBufferLayoutBindings.push_back(layoutBinding);
}

void VulkanLayout::setTexture(uint32_t binding, VulkanImagePtr image) {
  TextureLayoutBinding layoutBinding;
  layoutBinding.image = image;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_ALL_GRAPHICS;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  mTextureLayoutBindings.push_back(layoutBinding);
}

void VulkanLayout::setTexture(uint32_t binding, VulkanImagePtr image, VulkanSamplerPtr sampler) {
  TextureLayoutBinding layoutBinding;
  layoutBinding.image = image;
  layoutBinding.sampler = sampler;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  mTextureLayoutBindings.push_back(layoutBinding);
}

void VulkanLayout::update() {
  if (mVkDescriptorSets.empty())
    createDescriptorSets();

  int frame = 0;
  VkDevice device = mDevice->getVkDevice();
  for (auto descriptorSet : mVkDescriptorSets) {
    vector<VkWriteDescriptorSet> writeSets;

    for (int i = 0; i < mBufferLayoutBindings.size(); ++i) {
      VkWriteDescriptorSet writeSet = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0};
      writeSet.dstSet = descriptorSet;
      writeSet.dstBinding = mBufferLayoutBindings[i].binding.binding;
      writeSet.dstArrayElement = 0;
      writeSet.descriptorCount = 1;
      writeSet.descriptorType = mBufferLayoutBindings[i].binding.descriptorType;
      writeSet.pImageInfo = nullptr;
      writeSet.pBufferInfo = mBufferLayoutBindings.at(i).buffer->getVkDescriptorBufferInfo(frame);
      writeSet.pTexelBufferView = nullptr;
      writeSets.push_back(writeSet);
    }

    for (int i = 0; i < mTextureLayoutBindings.size(); ++i) {
      auto imageInfo = mTextureLayoutBindings.at(i).image->getVkDescriptorImageInfo(frame);
      if (mTextureLayoutBindings.at(i).sampler != nullptr)
        imageInfo->sampler = mTextureLayoutBindings.at(i).sampler->getVkSampler();

      VkWriteDescriptorSet writeSet = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0};
      writeSet.dstSet = descriptorSet;
      writeSet.dstBinding = mTextureLayoutBindings.at(i).binding.binding;
      writeSet.dstArrayElement = 0;
      writeSet.descriptorCount = 1;
      writeSet.descriptorType = mTextureLayoutBindings.at(i).binding.descriptorType;
      writeSet.pImageInfo = imageInfo;
      writeSet.pBufferInfo = nullptr;
      writeSet.pTexelBufferView = nullptr;
      writeSets.push_back(writeSet);
    }
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, 0);
    ++frame;
  }
}

VkDescriptorSetLayout VulkanLayout::getVkDescriptorSetLayout() {
  if (mVkDescriptorSetLayout == VK_NULL_HANDLE) {
    VkDescriptorSetLayoutBindings bindings;
    for (auto buffer : mBufferLayoutBindings)
      bindings.push_back(buffer.binding);
    for (auto texture : mTextureLayoutBindings)
      bindings.push_back(texture.binding);

    VkDescriptorSetLayoutCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, 0, 0};
    createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    createInfo.pBindings = bindings.data();

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateDescriptorSetLayout(device, &createInfo, 0, &mVkDescriptorSetLayout) != VK_SUCCESS) {
      cerr << "Error creating Descriptor Set Layout" << endl;
      mVkDescriptorSetLayout = VK_NULL_HANDLE;
    }
  }
  return mVkDescriptorSetLayout;
}

VkDescriptorPool VulkanLayout::getVkDescriptorPool() {
  if (mVkDescriptorPool == VK_NULL_HANDLE) {
    map<VkDescriptorType, int> typeMap;
    vector<VkDescriptorPoolSize> poolSizes;

    for (auto binding : mBufferLayoutBindings) {
      if (typeMap.count(binding.binding.descriptorType) == 0) {
        typeMap[binding.binding.descriptorType] = poolSizes.size();
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = binding.binding.descriptorType;
        poolSize.descriptorCount = 0;
        poolSizes.push_back(poolSize);
      }
      poolSizes[typeMap[binding.binding.descriptorType]].descriptorCount += mFrames;
    }

    for (auto binding : mTextureLayoutBindings) {
      if (typeMap.count(binding.binding.descriptorType) == 0) {
        typeMap[binding.binding.descriptorType] = poolSizes.size();
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = binding.binding.descriptorType;
        poolSize.descriptorCount = 0;
        poolSizes.push_back(poolSize);
      }
      poolSizes[typeMap[binding.binding.descriptorType]].descriptorCount += mFrames;
    }

    VkDescriptorPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 0};
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = static_cast<uint32_t>(mFrames);
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateDescriptorPool(device, &createInfo, nullptr, &mVkDescriptorPool) != VK_SUCCESS) {
      cerr << "Error creating Descriptor Pool" << endl;
      mVkDescriptorPool = VK_NULL_HANDLE;
    }
  }
  return mVkDescriptorPool;
}

VkDescriptorSet VulkanLayout::getVkDescriptorSet(int frame) {
  if (mVkDescriptorSets.empty())
    createDescriptorSets();
  return mVkDescriptorSets.at(frame);
}

void VulkanLayout::destroy() {
  clearDescriptorSets();
  destroyDescriptorPool();
  destroyDescriptorSetLayout();
  mBufferLayoutBindings.clear();
  mTextureLayoutBindings.clear();
}

void VulkanLayout::createDescriptorSets() {
  VkDevice device = mDevice->getVkDevice();

  auto layout = getVkDescriptorSetLayout();
  auto pool = getVkDescriptorPool();
  vector<VkDescriptorSetLayout> layouts(mFrames, layout);
  if (pool != VK_NULL_HANDLE && layout != VK_NULL_HANDLE) {
    VkDescriptorSetAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, 0};
    allocateInfo.descriptorPool = pool;
    allocateInfo.descriptorSetCount = static_cast<uint32_t>(mFrames);
    allocateInfo.pSetLayouts = layouts.data();

    mVkDescriptorSets.resize(mFrames);
    if (vkAllocateDescriptorSets(device, &allocateInfo, mVkDescriptorSets.data()) != VK_SUCCESS)
      throw runtime_error("Error Allocating DescriptorSets");
  }
}

void VulkanLayout::clearDescriptorSets() {
  if (mVkDescriptorPool != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkFreeDescriptorSets(
      device,
      mVkDescriptorPool,
      static_cast<uint32_t>(mFrames),
      mVkDescriptorSets.data()
    );
  }
  mVkDescriptorSets.clear();
}

void VulkanLayout::destroyDescriptorPool() {
  if (mVkDescriptorPool != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyDescriptorPool(device, mVkDescriptorPool, 0);
    mVkDescriptorPool = VK_NULL_HANDLE;
  }
}

void VulkanLayout::destroyDescriptorSetLayout() {
  if (mVkDescriptorSetLayout != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyDescriptorSetLayout(device, mVkDescriptorSetLayout, nullptr);
    mVkDescriptorSetLayout = VK_NULL_HANDLE;
  }
}
