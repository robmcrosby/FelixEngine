//
//  GpuContext.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef GpuContext_hpp
#define GpuContext_hpp

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <FelixEngine/VulkanTypes.hpp>
#include <iostream>
#include <memory>

namespace Felix {
  
class GpuContext;
typedef std::shared_ptr<GpuContext> GpuContextPtr;

class GpuContext {
private:
  SDL_Window* mWindow;
  
  VulkanDevicePtr mDevice;
  VulkanQueuePtr  mQueue;
  
  VulkanSwapChainPtr   mSwapChain;
  VulkanFrameBufferPtr mFramebuffer;
  VulkanFrameSyncPtr   mFrameSync;
  VulkanRenderPassPtr  mRenderPass;
  
  VulkanCommandPtr mCommand;
  
public:
  GpuContext();
  ~GpuContext();
  
  bool init(StringRef appName);
  
private:
  CStrings getSDLExtenstions() const;
  
public:
  static GpuContextPtr create();
};

} /* Felix */

#endif /* GpuContext_hpp */
