//
//  GpuContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#include "GpuContext.hpp"
#include "Parent.hpp"
#include <SDL3/SDL_vulkan.h>


using namespace std;
using namespace glm;
namespace Felix {

GpuContextPtr GpuContext::create() {
  GpuContextPtr context = make_shared<GpuContext>();
  return context;
}

GpuContext::GpuContext(): mPaused(false) {
  
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

void GpuContext::draw(Scene& scene) {
  if (mPaused)
    return;
  
  int frame = mSwapChain->getNextFrame(mFrameSync);
  if (frame >= 0) {
    updateLayouts(frame, scene);
    recordCommand(frame, scene);

    // Submit Command Buffer and Present Frame
    mQueue->submitCommand(mCommand, mFrameSync);
    mSwapChain->presentFrame(mFrameSync, mQueue);
  }
}

void GpuContext::resize(Scene& scene) {
  mPaused = true;
  mQueue->waitIdle();
  
  // Rebuild SwapChain and Render Passes
  mSwapChain->rebuild();
  mRenderPass->rebuild();
  
  // Rebuild the Render Pipelines
  set<VulkanPipeline*> pipelines;
  scene.registry().view<GpuDraw>().each([&](auto entity, auto& draw) {
    if (draw.pipeline)
      pipelines.insert(draw.pipeline.get());
  });
  for (auto pipeline : pipelines)
    pipeline->rebuild();
  mPaused = false;
}

void GpuContext::pause() {
  mPaused = true;
}

void GpuContext::resume(Scene& scene) {
  resize(scene);
}

glm::vec2 GpuContext::getWindowSize() const {
  int width, height;
  SDL_GetWindowSize(mWindow, &width, &height);
  return vec2(width, height);
}

Entity GpuContext::getMainPass(Scene& scene, StringRef name) const {
  // Check for an existing Main Render Pass
  auto items = scene.registry().view<GpuPass>();
  for (auto [item, pass] : items.each()) {
    if (pass.pass == mRenderPass)
      return {item, &scene};
  }
  
  // Add the Main Render Pass
  auto item = scene.add(name);
  auto& pass = item.add<GpuPass>();
  pass.pass = mRenderPass;
  pass.layout = mDevice->createLayout(mSwapChain->frames());
  return item;
}

Entity GpuContext::getMainCamera(Scene& scene) const {
  auto item = getMainPass(scene, "MainCamera");
  if (!item.has<Camera>()) {
    auto& pass = item.get<GpuPass>();
    auto& camera = item.get<Camera>();
    assert(pass.layout->setStorage(0, camera));
  }
  return item;
}

Entity GpuContext::addDrawItem(Entity pass, StringRef name) const {
  auto item = pass.addChild(name);
  
  // Get Draw Component
  auto& draw = item.get<GpuDraw>();
  draw.layoutSet = mDevice->createLayoutSet(mSwapChain->frames());
  
  // Add Transform Component
  auto& transform = item.get<Transform>();
  assert(draw.layoutSet->at(0)->setStorage(0, transform));
  
  // Add Render Pass Layout
  draw.layoutSet->add(pass.get<GpuPass>().layout);
  return item;
}

bool GpuContext::setShaders(Entity item, StringRef vertexFile, StringRef fragmentFile) const {
  auto& draw = item.get<GpuDraw>();
  draw.pipeline = mDevice->createPipeline();
  return draw.pipeline->setVertexShader(vertexFile) && draw.pipeline->setFragmentShader(fragmentFile);
}

void GpuContext::updateLayouts(int frame, Scene& scene) {
  // Update Cameras
  auto cameras = scene.registry().view<GpuPass, Camera>();
  for (auto [item, pass, camera] : cameras.each()) {
    pass.layout->update(0, camera, frame);
  }
  
  // Update Object Transforms
  auto transforms = scene.registry().view<GpuDraw, Transform>();
  for (auto [item, draw, transform] : transforms.each()) {
    draw.layoutSet->at(0)->update(0, transform, frame);
  }
}

void GpuContext::recordCommand(int frame, Scene& scene) {
  auto passes = scene.registry().view<GpuPass>();
  auto draws = scene.registry().view<GpuDraw, Parent>();
  
  // Record the Command
  mCommand->begin(frame);
  for (auto [passItem, pass] : passes.each()) {
    if (pass.pass && pass.visible) {
      mCommand->beginRenderPass(pass.pass);
      for (auto [drawItem, draw, parent] : draws.each()) {
        if (parent.parent == passItem && draw.visible) {
          //if (drawComp.guiContext && drawComp.visible) {
          //  drawComp.guiContext->draw(mCommand->getVkCommandBuffer(frame), scene);
          //}
          //else
          if (draw.visible && draw.instances > 0) {
            mCommand->bind(draw.pipeline, pass.pass, draw.mesh, draw.layoutSet);
            mCommand->draw(draw.mesh, draw.instances);
          }
        }
      }
      mCommand->endRenderPass();
    }
  }
  mCommand->end();
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
