//
//  GpuContext.hpp
//  SDLGame
//
//  Created by Robert Crosby on 3/27/26.
//

#ifndef GpuContext_hpp
#define GpuContext_hpp

#include <stdio.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include "VulkanIncludes.hpp"
#include "EcsEntity.hpp"


class EcsScene;

class GpuContext;
typedef std::shared_ptr<GpuContext> GpuContextPtr;

class GpuContext {
private:
  SDL_Window* mWindow;
  
  uint64_t mLastTime;
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
  
  bool init();
  void destory();
  
  void draw(EcsScene& scene);
  void resize(EcsScene& scene);
  
  glm::vec2 getWindowSize() const;
  
  void pause();
  void resume(EcsScene& scene);
  
  VulkanDevicePtr getDevice() const { return mDevice; }
  VulkanQueuePtr getQueue() const { return mQueue; }
  
  VulkanRenderPassPtr getRenderPass() const { return mRenderPass; }
  VulkanSwapChainPtr getSwapChain() const { return mSwapChain; }
  
  EcsEntity getMainPass(EcsScene& scene, StringRef name = "MainPass") const;
  EcsEntity getMainCamera(EcsScene& scene) const;
  
  EcsEntity addDrawItem(EcsEntity pass) const;
  bool setShaders(EcsEntity item, StringRef vertexFile, StringRef fragmentFile) const;
  
  template <typename T>
  VulkanMeshPtr loadMesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices) {
    auto mesh = mDevice->createMesh();
    assert(mesh->addBuffer(mQueue, vertices));
    assert(mesh->setIndexBuffer(mQueue, indices));
    return mesh;
  }
  
  template <typename T>
  VulkanMeshPtr setMesh(EcsEntity item, const std::vector<T>& vertices, const std::vector<uint32_t>& indices) {
    auto mesh = loadMesh(vertices, indices);
    item.get<GpuDrawComp>().mesh = mesh;
    return mesh;
  }
  
public:
  static GpuContextPtr create();
  
private:
  void updateLayouts(int frame, EcsScene& scene);
  void recordCommand(int frame, EcsScene& scene);
  CStrings getSDLExtenstions() const;
};


#endif /* GpuContext_hpp */
