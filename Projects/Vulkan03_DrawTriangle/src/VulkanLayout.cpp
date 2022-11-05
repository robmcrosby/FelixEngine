#include "VulkanIncludes.hpp"


using namespace std;


VulkanLayout::VulkanLayout(VulkanDevice* device):
  mDevice(device),
  mVkDescriptorSetLayout(VK_NULL_HANDLE),
  mVkDescriptorPool(VK_NULL_HANDLE),
  mVkDescriptorSet(VK_NULL_HANDLE) {

}

VulkanLayout::~VulkanLayout() {
  destroy();
}

void VulkanLayout::setBuffer(VulkanBufferPtr buffer, uint32_t binding) {
  BufferLayoutBinding layoutBinding;
  layoutBinding.buffer = buffer;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_ALL_GRAPHICS;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  layoutBinding.bufferInfo.buffer = buffer->getVkBuffer();
  layoutBinding.bufferInfo.offset = 0;
  layoutBinding.bufferInfo.range = VK_WHOLE_SIZE;
  mBufferLayoutBindings.push_back(layoutBinding);
}

void VulkanLayout::setTexture(VulkanImagePtr image, uint32_t binding) {
  TextureLayoutBinding layoutBinding;
  layoutBinding.image = image;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_ALL_GRAPHICS;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  layoutBinding.imageInfo.sampler = VK_NULL_HANDLE;
  layoutBinding.imageInfo.imageView = image->getVkImageView();
  layoutBinding.imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  mTextureLayoutBindings.push_back(layoutBinding);
}

void VulkanLayout::update() {
  auto descriptorSet = getVkDescriptorSet();
  vector<VkWriteDescriptorSet> writeSets;

  for (int i = 0; i < mBufferLayoutBindings.size(); ++i) {
    VkWriteDescriptorSet writeSet = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0};
    writeSet.dstSet = descriptorSet;
    writeSet.dstBinding = mBufferLayoutBindings[i].binding.binding;
    writeSet.dstArrayElement = 0;
    writeSet.descriptorCount = 1;
    writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeSet.pImageInfo = nullptr;
    writeSet.pBufferInfo = &mBufferLayoutBindings[i].bufferInfo;
    writeSet.pTexelBufferView = nullptr;
    writeSets.push_back(writeSet);
  }

  for (int i = 0; i < mTextureLayoutBindings.size(); ++i) {
    VkWriteDescriptorSet writeSet = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, 0};
    writeSet.dstSet = descriptorSet;
    writeSet.dstBinding = mTextureLayoutBindings[i].binding.binding;
    writeSet.dstArrayElement = 0;
    writeSet.descriptorCount = 1;
    writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writeSet.pImageInfo = &mTextureLayoutBindings[i].imageInfo;
    writeSet.pBufferInfo = nullptr;
    writeSet.pTexelBufferView = nullptr;
    writeSets.push_back(writeSet);
  }

  VkDevice device = mDevice->getVkDevice();
  vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, 0);
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
    vector<VkDescriptorPoolSize> poolSizes;

    if (mBufferLayoutBindings.size() > 0) {
      VkDescriptorPoolSize poolSize = {};
      poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      poolSize.descriptorCount = static_cast<uint32_t>(mBufferLayoutBindings.size());
      poolSizes.push_back(poolSize);
    }

    if (mTextureLayoutBindings.size() > 0) {
      VkDescriptorPoolSize poolSize = {};
      poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
      poolSize.descriptorCount = static_cast<uint32_t>(mTextureLayoutBindings.size());
      poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 0};
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = 1;
    createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();

    VkDevice device = mDevice->getVkDevice();
    if (vkCreateDescriptorPool(device, &createInfo, 0, &mVkDescriptorPool) != VK_SUCCESS) {
      cerr << "Error creating Descriptor Pool" << endl;
      mVkDescriptorPool = VK_NULL_HANDLE;
    }
  }
  return mVkDescriptorPool;
}

VkDescriptorSet VulkanLayout::getVkDescriptorSet() {
  if (mVkDescriptorSet == VK_NULL_HANDLE) {
    auto layout = getVkDescriptorSetLayout();
    auto pool = getVkDescriptorPool();
    if (pool != VK_NULL_HANDLE && layout != VK_NULL_HANDLE) {
      VkDescriptorSetAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, 0};
      allocateInfo.descriptorPool = pool;
      allocateInfo.descriptorSetCount = 1;
      allocateInfo.pSetLayouts = &layout;

      VkDevice device = mDevice->getVkDevice();
      if (vkAllocateDescriptorSets(device, &allocateInfo, &mVkDescriptorSet) != VK_SUCCESS) {
        cerr << "Error Allocating DescriptorSet" << endl;
        mVkDescriptorSet = VK_NULL_HANDLE;
      }
    }
  }
  return mVkDescriptorSet;
}

void VulkanLayout::destroy() {
  freeDescriptorSet();
  destroyDescriptorPool();
  destroyDescriptorSetLayout();
  mBufferLayoutBindings.clear();
  mTextureLayoutBindings.clear();
}

void VulkanLayout::freeDescriptorSet() {
  if (mVkDescriptorSet != VK_NULL_HANDLE) {
    if (mVkDescriptorPool != VK_NULL_HANDLE) {
      VkDevice device = mDevice->getVkDevice();
      vkFreeDescriptorSets(device, mVkDescriptorPool, 1, &mVkDescriptorSet);
    }
    mVkDescriptorSet = VK_NULL_HANDLE;
  }
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
