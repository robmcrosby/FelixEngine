#include <VulkanTypes.hpp>

#ifndef VulkanFrameSync_hpp
#define VulkanFrameSync_hpp


class VulkanFrameSync {
  VulkanDevice* mDevice;
  VkSemaphores  mAvailableSemaphores;
  VkSemaphores  mFinishedSemaphores;
  VkFences      mInFlightFences;
  VkFences      mImagesInFlight;
  int           mFrameInFlight;
  int           mCurrentFrame;


public:
  VulkanFrameSync(VulkanDevice* device);
  ~VulkanFrameSync();

  void setup(int frames, int framesInflight);
  void destroy();

  void waitForInFlight();
  void resetInFlight();
  void assignInFlight(int frame);
  int nextInFlight();

  int currentFrame() const {return mCurrentFrame;}
  int currentInFlight() const {return mFrameInFlight;}
  VkSemaphore availableSemaphore() {return mAvailableSemaphores.at(mFrameInFlight);}
  VkSemaphore finishedSemaphore() {return mFinishedSemaphores.at(mFrameInFlight);}
  VkFence inFlightFence() {return mInFlightFences.at(mFrameInFlight);}
};


#endif /* VulkanFrameSync_hpp */
