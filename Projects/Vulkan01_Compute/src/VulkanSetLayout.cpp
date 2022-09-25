#include "VulkanIncludes.hpp"


using namespace std;


VulkanSetLayout::VulkanSetLayout(VulkanDevice* device):
  mDevice(device),
  mVkDescriptorSetLayout(VK_NULL_HANDLE),
  mVkDescriptorSet(VK_NULL_HANDLE) {

}

VulkanSetLayout::~VulkanSetLayout() {
  destroy();
}

void VulkanSetLayout::setBuffer(VulkanBufferPtr buffer, uint32_t binding) {
  LayoutBinding layoutBinding;
  layoutBinding.buffer = buffer;
  layoutBinding.binding.binding = binding;
  layoutBinding.binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layoutBinding.binding.descriptorCount = 1;
  layoutBinding.binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
  layoutBinding.binding.pImmutableSamplers = nullptr;
  mLayoutBindings.push_back(layoutBinding);
}

bool VulkanSetLayout::init() {
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
  return mVkDescriptorSetLayout != VK_NULL_HANDLE;
}

void VulkanSetLayout::destroy() {
  mLayoutBindings.clear();

  if (mVkDescriptorSetLayout != VK_NULL_HANDLE) {
    VkDevice device = mDevice->getVkDevice();
    vkDestroyDescriptorSetLayout(device, mVkDescriptorSetLayout, nullptr);
    mVkDescriptorSetLayout = VK_NULL_HANDLE;
  }
}
