//
//  GpuContext.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#include "GpuContext.hpp"
#include "GuiContext.hpp"
#include "VulkanMesh.hpp"
#include "Parent.hpp"
#include "ObjLoader.hpp"
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
  pause();
  
  // Rebuild SwapChain and Main RenderPass
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
}

void GpuContext::pause() {
  mPaused = true;
  mQueue->waitIdle();
}

void GpuContext::resume(Scene& scene) {
  mPaused = false;
}

glm::vec2 GpuContext::getWindowSize() const {
  int width, height;
  SDL_GetWindowSize(mWindow, &width, &height);
  return vec2(width, height);
}

Entity GpuContext::getMainPass(Scene& scene) const {
  // Check for an existing Main Render Pass
  auto items = scene.registry().view<GpuPass>();
  for (auto [item, pass] : items.each()) {
    if (pass.renderPass == mRenderPass)
      return {item, &scene};
  }
  
  // Add the Main Render Pass
  auto item = scene.add("MainPass");
  auto& pass = item.add<GpuPass>();
  pass.renderPass = mRenderPass;
  pass.layout = mDevice->createLayout(mSwapChain->frames());
  return item;
}

Entity GpuContext::getMainCamera(Scene& scene) const {
  auto item = getMainPass(scene);
  if (!item.has<Camera>()) {
    auto& camera = item.add<Camera>();
    auto& pass = item.get<GpuPass>();
    assert(pass.layout->setStorage(0, camera));
  }
  return item;
}

Entity GpuContext::createPass(Entity parent, StringRef name) const {
  auto item = parent.addChild(name);
  auto& pass = item.add<GpuPass>();
  pass.renderPass = mDevice->createRenderPass();
  pass.renderPass->setFramebuffer(mDevice->createFrameBuffer());
  pass.layout = mDevice->createLayout(mSwapChain->frames());
  return item;
}

Entity GpuContext::createCamera(Entity parent, StringRef name) const {
  auto item = createPass(parent, name);
  auto& camera = item.add<Camera>();
  auto& pass = item.get<GpuPass>();
  assert(pass.layout->setStorage(0, camera));
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

Entity GpuContext::loadModel(Entity pass, StringRef file) const {
  auto drawItem = addDrawItem(pass, file);
  assert(setShaders(drawItem, "StaticVertex.spv", "DrawNormals.spv"));
  
  StaticModel model;
  assert(ObjLoader::loadStaticMesh(model, file));
  
  auto mesh = setMesh(drawItem, model.vertices, model.indices);
  mesh->addAttribute(0, 0, 3, 0); // Position
  mesh->addAttribute(0, 1, 3, 3); // Normal
  mesh->addAttribute(0, 2, 2, 6); // Texcoord
  
  return drawItem;
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
  auto passes = scene.registry().view<GpuPass, Parent>();
  auto mainCamera = getMainPass(scene);
  auto& mainPass = mainCamera.get<GpuPass>();
  auto draws = scene.registry().view<GpuDraw, Parent>();
  
  // Begin recording the Command
  mCommand->begin(frame);
  
  // Draw any sub-passes of the main pass
  for (auto [passItem, pass, parent] : passes.each()) {
    if (parent == mainCamera) {
      mCommand->beginRenderPass(pass.renderPass);
      for (auto [drawItem, draw, parent] : draws.each()) {
        if (parent == passItem && draw.visible) {
          if (draw.guiContext)
            draw.guiContext->draw(mCommand->getVkCommandBuffer(frame), scene);
          else
            recordDraw(mCommand, pass, draw);
        }
      }
      mCommand->endRenderPass();
    }
  }
  
  // Draw the Main Render Pass
  mCommand->beginRenderPass(mainPass.renderPass);
  for (auto [drawItem, draw, parent] : draws.each()) {
    if (parent == mainCamera && draw.visible) {
      if (draw.guiContext)
        draw.guiContext->draw(mCommand->getVkCommandBuffer(frame), scene);
      else
        recordDraw(mCommand, mainPass, draw);
    }
  }
  mCommand->endRenderPass();
  mCommand->end();
}

void GpuContext::recordDraw(VulkanCommandPtr command, GpuPass& pass, GpuDraw& draw) {
  if (draw.instances > 0) {
    command->bind(draw.pipeline, pass.renderPass, draw.mesh, draw.layoutSet);
    command->draw(draw.mesh, draw.instances);
  }
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
