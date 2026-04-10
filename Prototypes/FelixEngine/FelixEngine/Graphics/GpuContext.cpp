//
//  GpuContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#include "GpuContext.hpp"
#include "VulkanIncludes.hpp"
#include <SDL3/SDL_vulkan.h>


using namespace std;
namespace Felix {

GpuContextPtr GpuContext::create() {
  GpuContextPtr context = make_shared<GpuContext>();
  return context;
}

GpuContext::GpuContext() {
  
}

GpuContext::~GpuContext() {
  
}

bool GpuContext::init(StringRef appName) {
  // Create the main window
  mWindow = SDL_CreateWindow("SDLGame", 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS);
  if (!mWindow) {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return false;
  }
  
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName(appName);
  instance.setEngineName("Felix Engine");
  instance.enableValidation();
  
  CStrings sdlExtensions = getSDLExtenstions();
  sdlExtensions.push_back("VK_EXT_swapchain_colorspace");
  for (string extension : sdlExtensions)
    instance.addExtension(extension);
  
  if (!instance.init()) {
    cerr << "Error initalizing Vulkan Instance!" << endl;
    return false;
  }
  
  mDevice = instance.pickDevice();
  if (!mDevice) {
    cerr << "Error Creating Vulkan Device!" << endl;
    return false;
  }
  
  mDevice->addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  mQueue = mDevice->createQueue(VK_QUEUE_GRAPHICS_BIT);
  if (!mDevice->init()) {
    cerr << "Error Initalizing Vulkan Device!" << endl;
    return false;
  }
  
  mSwapChain = mDevice->createSwapChain();
  mSwapChain->setToWindow(mWindow);
  
  mFramebuffer = mDevice->createFrameBuffer();
  mFramebuffer->addColorAttachment(mSwapChain->getPresentImage());
  
  mRenderPass = mDevice->createRenderPass();
  mRenderPass->setFramebuffer(mFramebuffer);
  
  mFrameSync = mDevice->createFrameSync();
  mFrameSync->setup(mSwapChain->frames(), 2);

  mCommand = mQueue->createCommand(mSwapChain->frames());
  
  
  return true;
}

CStrings GpuContext::getSDLExtenstions() const {
  unsigned int count;
  auto exts = SDL_Vulkan_GetInstanceExtensions(&count);
  
  CStrings extensions;
  for (int i = 0; i < count; ++i)
    extensions.push_back(exts[i]);
  return extensions;
}

} /* Felix */
