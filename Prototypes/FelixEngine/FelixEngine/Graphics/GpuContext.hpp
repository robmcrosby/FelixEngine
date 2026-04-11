//
//  GpuContext.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef GpuContext_hpp
#define GpuContext_hpp

#include <FelixEngine/Scene.hpp>
#include <FelixEngine/VulkanIncludes.hpp>
#include <FelixEngine/GpuComponents.hpp>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <memory>


namespace Felix {
  
class GpuContext;
typedef std::shared_ptr<GpuContext> GpuContextPtr;

class GpuContext {
private:
  SDL_Window* mWindow;
  bool mPaused;
  
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
  
  void draw(Scene& scene);
  void resize(Scene& scene);
  void pause();
  void resume(Scene& scene);
  
  glm::vec2 getWindowSize() const;
  
  VulkanDevicePtr getDevice() const { return mDevice; }
  VulkanQueuePtr getQueue() const { return mQueue; }
  
  VulkanRenderPassPtr getRenderPass() const { return mRenderPass; }
  VulkanSwapChainPtr getSwapChain() const { return mSwapChain; }
  
  Entity getMainPass(Scene& scene, StringRef name = "MainPass") const;
  Entity getMainCamera(Scene& scene) const;
  
  Entity addDrawItem(Entity pass, StringRef name = "") const;
  bool setShaders(Entity item, StringRef vertexFile, StringRef fragmentFile) const;
  
  template <typename T>
  VulkanMeshPtr loadMesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices) {
    auto mesh = mDevice->createMesh();
    assert(mesh->addBuffer(mQueue, vertices));
    assert(mesh->setIndexBuffer(mQueue, indices));
    return mesh;
  }
  
  template <typename T>
  VulkanMeshPtr setMesh(Entity item, const std::vector<T>& vertices, const std::vector<uint32_t>& indices) {
    auto mesh = loadMesh(vertices, indices);
    item.get<GpuDraw>().mesh = mesh;
    return mesh;
  }
  
private:
  void updateLayouts(int frame, Scene& scene);
  void recordCommand(int frame, Scene& scene);
  CStrings getSDLExtenstions() const;
  
public:
  static GpuContextPtr create();
};

} /* Felix */

#endif /* GpuContext_hpp */
