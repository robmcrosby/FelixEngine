#include <VulkanIncludes.hpp>


using namespace std;


VulkanSampler::VulkanSampler(VulkanDevice* device):
  mDevice(device),
  mVkSampler(VK_NULL_HANDLE),
  mMinFilter(VK_FILTER_NEAREST),
  mMagFilter(VK_FILTER_NEAREST),
  mMipFilter(VK_FILTER_NEAREST),
  mMipLevels(1) {

}

VulkanSampler::~VulkanSampler() {

}

void VulkanSampler::setFilters(VkFilter min, VkFilter mag, VkFilter mip) {
  mVkSampler = VK_NULL_HANDLE;
  mMinFilter = min;
  mMagFilter = mag;
  mMipFilter = mip;
}

VkSampler VulkanSampler::getVkSampler() {
  if (mVkSampler == VK_NULL_HANDLE) {
    auto& samplerMap = mDevice->getVkSamplerMap();
    uint32_t key = getKey();
    if (!samplerMap.count(key))
      samplerMap[key] = createVkSampler();
    mVkSampler = samplerMap.at(key);
  }
  return mVkSampler;
}

VkSampler VulkanSampler::createVkSampler() const {
  VkSamplerCreateInfo samplerInfo{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, 0, 0};
  samplerInfo.magFilter = mMagFilter;
  samplerInfo.minFilter = mMinFilter;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.maxAnisotropy = 1.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = mMipLevels > 1 ? static_cast<float>(mMipLevels) : 0.0f;

  auto device = mDevice->getVkDevice();
  VkSampler sampler = VK_NULL_HANDLE;
  if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    throw runtime_error("failed to create texture sampler!");
  return sampler;
}

uint32_t VulkanSampler::getKey() const {
  return mMinFilter | (mMagFilter << 1) | (mMipFilter << 2);
}
