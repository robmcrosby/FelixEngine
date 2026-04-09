//
//  GuiContext.hpp
//  SDLGame
//
//  Created by Robert Crosby on 3/21/26.
//

#ifndef ImGuiContext_hpp
#define ImGuiContext_hpp

#include <stdio.h>
#include <memory>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>


class ImDrawData;
class EcsScene;
class EcsEntity;

class GuiPanelComp;
class GuiBoundsComp;
class GuiWidgetComp;

class GpuContext;

class GuiContext;
typedef std::shared_ptr<GuiContext> GuiContextPtr;

class GuiContext {
private:
  VkDevice mDevice;
  VkDescriptorPool mDescriptorPool;
  
  mutable bool mCheckboxSelection;
  mutable float mFloatValue;
  mutable int mButtonCount;
  
public:
  GuiContext();
  ~GuiContext();
  
  bool init(GpuContext& gpuContext);
  void destory();
  
  ImDrawData* drawUI(EcsScene& scene) const;
  
  void draw(VkCommandBuffer cmd, EcsScene& scene) const;
  bool handle(SDL_Event *event);
  
public:
  static GuiContextPtr create();
  
private:
  void setNextPanel(const GuiPanelComp& panel) const;
  void beginPanel(GuiPanelComp& panel, const std::string& name) const;
  void endPanel(GuiBoundsComp& bounds) const;
  
  void addWidget(EcsEntity widget, GuiWidgetComp& widgetComp) const;
  
  bool setupDescriptorPool();
};


#endif /* ImGuiContext_hpp */
