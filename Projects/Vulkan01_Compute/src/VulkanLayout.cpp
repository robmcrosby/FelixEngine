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
  LayoutBinding layoutBinding;
  layoutBinding.buffer = buffer;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  layoutBinding.bufferInfo.buffer = buffer->getVkBuffer();
  layoutBinding.bufferInfo.offset = 0;
  layoutBinding.bufferInfo.range = VK_WHOLE_SIZE;
  mLayoutBindings.push_back(layoutBinding);
}

void VulkanLayout::update() {
  vector<VkWriteDescriptorSet> writeSets(mLayoutBindings.size());
  auto descriptorSet = getVkDescriptorSet();

  for (int i = 0; i < mLayoutBindings.size(); ++i) {
    writeSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeSets[i].pNext = 0;
    writeSets[i].dstSet = descriptorSet;
    writeSets[i].dstBinding = mLayoutBindings[i].binding.binding;
    writeSets[i].dstArrayElement = 0;
    writeSets[i].descriptorCount = 1;
    writeSets[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeSets[i].pImageInfo = nullptr;
    writeSets[i].pBufferInfo = &mLayoutBindings[i].bufferInfo;
    writeSets[i].pTexelBufferView = nullptr;
  }

  VkDevice device = mDevice->getVkDevice();
  vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, 0);
}

VkDescriptorSetLayout VulkanLayout::getVkDescriptorSetLayout() {
  if (mVkDescriptorSetLayout == VK_NULL_HANDLE) {
    vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto layoutBinding : mLayoutBindings)
      bindings.push_back(layoutBinding.binding);

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
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(mLayoutBindings.size());

    VkDescriptorPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 0};
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    createInfo.maxSets = 1;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;

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
  mLayoutBindings.clear();
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