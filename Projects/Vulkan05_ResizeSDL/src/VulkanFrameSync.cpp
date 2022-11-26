#include "VulkanIncludes.hpp"


using namespace std;


VulkanFrameSync::VulkanFrameSync(VulkanDevice* device):
  mDevice(device),
  mCurrentFrame(0),
  mFrameInFlight(0) {

}

VulkanFrameSync::~VulkanFrameSync() {
  destroy();
}

void VulkanFrameSync::setup(int frames, int framesInflight) {
  mFrameInFlight = 0;

  mAvailableSemaphores.resize(framesInflight);
  mFinishedSemaphores.resize(framesInflight);
  mInFlightFences.resize(framesInflight);
  mImagesInFlight.resize(frames, VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
  VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  auto device = mDevice->getVkDevice();
  for (int i = 0; i < framesInflight; i++) {
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {
      throw runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}

void VulkanFrameSync::destroy() {
  auto device = mDevice->getVkDevice();
  for (int i = 0; i < mInFlightFences.size(); i++) {
    vkDestroySemaphore(device, mFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(device, mAvailableSemaphores[i], nullptr);
    vkDestroyFence(device, mInFlightFences[i], nullptr);
  }
  mAvailableSemaphores.clear();
  mFinishedSemaphores.clear();
  mInFlightFences.clear();
  mImagesInFlight.clear();
}

void VulkanFrameSync::waitForInFlight() {
  vkWaitForFences(mDevice->getVkDevice(), 1, &mInFlightFences[mFrameInFlight], VK_TRUE, UINT64_MAX);
}

void VulkanFrameSync::resetInFlight() {
  vkResetFences(mDevice->getVkDevice(), 1, &mInFlightFences[mFrameInFlight]);
}

void VulkanFrameSync::assignInFlight(int frame) {
  mCurrentFrame = frame;
  if (mImagesInFlight[frame] != VK_NULL_HANDLE)
    vkWaitForFences(mDevice->getVkDevice(), 1, &mImagesInFlight[frame], VK_TRUE, UINT64_MAX);

  mImagesInFlight[frame] = mInFlightFences[mFrameInFlight];
}

int VulkanFrameSync::nextInFlight() {
  return mFrameInFlight = (mFrameInFlight + 1) % ((int)mInFlightFences.size());
}
