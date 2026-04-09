//
//  GpuContext.cpp
//  SDLGame
//
//  Created by Robert Crosby on 3/27/26.
//

#include "GpuContext.hpp"
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "EcsEntity.hpp"


using namespace std;
using namespace glm;


GpuContext::GpuContext(): mPaused(false) {
  
}

GpuContext::~GpuContext() {
  destory();
}

GpuContextPtr GpuContext::create() {
  GpuContextPtr context = make_shared<GpuContext>();
  return context;
}

bool GpuContext::init() {
  //mScene = &scene;
  
  // Create the main window
  mWindow = SDL_CreateWindow("SDLGame", 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS);
  if (!mWindow) {
    cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return false;
  }
  
  auto& instance = VulkanInstance::Get();
  instance.setApplicationName("SDLGame");
  instance.setEngineName("No Engine");
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

void GpuContext::updateLayouts(int frame, EcsScene &scene) {
  // Update Cameras
  auto cameras = scene.registry().view<GpuPassComp, CameraComp>();
  for (auto [item, passComp, cameraComp] : cameras.each()) {
    passComp.layout->update(0, cameraComp.camera, frame);
  }
  
  // Update Object Transforms
  auto transforms = scene.registry().view<GpuDrawComp, TransformComp>();
  for (auto [item, drawComp, transformComp] : transforms.each()) {
    drawComp.layoutSet->at(0)->update(0, transformComp.transform, frame);
  }
}

void GpuContext::recordCommand(int frame, EcsScene& scene) {
  auto passes = scene.registry().view<GpuPassComp>();
  auto drawables = scene.registry().view<GpuDrawComp>();
  
  // Record the Command
  mCommand->begin(frame);
  for (auto [pass, passComp] : passes.each()) {
    if (passComp.pass && passComp.visible) {
      mCommand->beginRenderPass(passComp.pass);
      for (auto [item, drawComp] : drawables.each()) {
        if (drawComp.pass == pass && drawComp.visible) {
          if (drawComp.guiContext && drawComp.visible) {
            drawComp.guiContext->draw(mCommand->getVkCommandBuffer(frame), scene);
          }
          else if (drawComp.visible && drawComp.instances > 0) {
            mCommand->bind(drawComp.pipeline, passComp.pass, drawComp.mesh, drawComp.layoutSet);
            mCommand->draw(drawComp.mesh, drawComp.instances);
          }
        }
      }
      mCommand->endRenderPass();
    }
  }
  mCommand->end();
}

void GpuContext::destory() {
  if (mWindow) {
    SDL_DestroyWindow(mWindow);
    mWindow = NULL;
  }
}

void GpuContext::draw(EcsScene& scene) {
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

vec2 GpuContext::getWindowSize() const {
  int width, height;
  SDL_GetWindowSize(mWindow, &width, &height);
  return vec2(width, height);
}

void GpuContext::resize(EcsScene& scene) {
  mPaused = true;
  
  mQueue->waitIdle();
  mSwapChain->rebuild();
  
  // Rebuild Render Pass
  mRenderPass->rebuild();
  
  set<VulkanPipeline*> pipelines;
  auto drawables = scene.registry().view<GpuDrawComp>();
  for (auto [item, drawComp] : drawables.each()) {
    if (drawComp.pipeline)
      pipelines.insert(drawComp.pipeline.get());
  }
  for (auto pipeline : pipelines)
    pipeline->rebuild();
  mPaused = false;
  
  scene.onWindowResize(getWindowSize());
}

void GpuContext::pause() {
  mPaused = true;
}

void GpuContext::resume(EcsScene& scene) {
  resize(scene);
}

EcsEntity GpuContext::getMainPass(EcsScene& scene, StringRef name) const {
  // Check for an existing Main Render Pass
  auto passes = scene.registry().view<GpuPassComp>();
  for (auto [pass, passComp] : passes.each()) {
    if (passComp.pass == mRenderPass)
      return {pass, &scene};
  }
  
  // Add the Main Render Pass
  auto passItem = scene.create(name);
  auto& passComp = passItem.get<GpuPassComp>();
  passComp.pass = mRenderPass;
  passComp.layout = mDevice->createLayout(mSwapChain->frames());
  return passItem;
}

EcsEntity GpuContext::getMainCamera(EcsScene& scene) const {
  auto cameraItem = getMainPass(scene, "MainCamera");
  if (!cameraItem.has<CameraComp>()) {
    auto& passComp = cameraItem.get<GpuPassComp>();
    auto& cameraComp = cameraItem.get<CameraComp>();
    passComp.layout->setStorage(0, cameraComp.camera);
  }
  return cameraItem;
}

EcsEntity GpuContext::addDrawItem(EcsEntity pass) const {
  auto drawItem = pass.scene().create();
  auto& drawComp = drawItem.get<GpuDrawComp>();
  drawComp.layoutSet = mDevice->createLayoutSet(mSwapChain->frames());
  drawComp.pass = pass.handle();
  
  // Add Transform Component
  auto& transComp = drawItem.get<TransformComp>();
  assert(drawComp.layoutSet->at(0)->setStorage(0, transComp.transform));
  
  // Add Render Pass Layout
  drawComp.layoutSet->add(pass.get<GpuPassComp>().layout);
  
  return drawItem;
}

bool GpuContext::setShaders(EcsEntity item, StringRef vertexFile, StringRef fragmentFile) const {
  auto& drawComp = item.get<GpuDrawComp>();
  drawComp.pipeline = mDevice->createPipeline();
  return drawComp.pipeline->setVertexShader(vertexFile) && drawComp.pipeline->setFragmentShader(fragmentFile);
}

CStrings GpuContext::getSDLExtenstions() const {
  unsigned int count;
  auto exts = SDL_Vulkan_GetInstanceExtensions(&count);
  
  CStrings extensions;
  for (int i = 0; i < count; ++i)
    extensions.push_back(exts[i]);
  return extensions;
}
