//
//  GuiContext.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef GuiContext_hpp
#define GuiContext_hpp

#include <FelixEngine/Scene.hpp>
#include <FelixEngine/GuiComponents.hpp>
#include <iostream>
#include <memory>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>


class ImDrawData;
namespace Felix {

class GpuContext;
class GuiContext;
typedef std::shared_ptr<GuiContext> GuiContextPtr;

class GuiContext {
private:
  VkDevice mDevice;
  VkDescriptorPool mDescriptorPool;
  
public:
  GuiContext();
  ~GuiContext();
  
  bool init(GpuContext& gpuContext);
  void destory();
  
  Entity addGuiDrawToPass(Entity pass);
  
  ImDrawData* drawUI(Scene& scene) const;
  
  void draw(VkCommandBuffer cmd, Scene& scene) const;
  bool handle(SDL_Event *event);
  
public:
  static GuiContextPtr create();
  
private:
  void setNextPanel(const GuiPanel& panel) const;
  void beginPanel(GuiPanel& panel, const std::string& name) const;
  void endPanel(GuiBounds& bounds) const;
  
  void addWidget(Entity entity, GuiWidget& widget) const;
  
  bool setupDescriptorPool();
};

} /* Felix */


#endif /* GuiContext_hpp */
