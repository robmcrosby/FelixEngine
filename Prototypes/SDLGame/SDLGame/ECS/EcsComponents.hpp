//
//  EcsComponents.hpp
//  SDLGame
//
//  Created by Robert Crosby on 3/23/26.
//

#ifndef EcsComponents_hpp
#define EcsComponents_hpp

#include "entt.hpp"
#include <glm/glm.hpp>
#include "VulkanIncludes.hpp"
#include "GuiContext.hpp"
#include "UUID.hpp"

enum EcsEventType {
  UndefinedEvent = 0,
  ButtonPressEvent,
};

class EcsEntity;

struct IdComp {
  UUID uuid;
  
  IdComp() = default;
  IdComp(const IdComp&) = default;
  IdComp(uint64_t id): uuid(id) {}
};

struct TagComp {
  std::string tag;
  
  TagComp() = default;
  TagComp(const TagComp&) = default;
  TagComp(const std::string& t): tag(t) {}
  
  operator std::string& () { return tag; }
  operator const std::string& () const { return tag; }
};

struct ParentComp {
  entt::entity parent = entt::null;
};

struct Camera {
  glm::mat4 projection{1.0f};
  glm::mat4 view{1.0f};
  glm::vec3 position{0.0f};
};

struct CameraComp {
  Camera camera;
};

struct Transform {
  glm::mat4 model{1.0f};
};

struct TransformComp {
  Transform transform;
  
  TransformComp() = default;
  TransformComp(const TransformComp&) = default;
  TransformComp(const glm::mat4& m) {transform.model = m;}
  
  operator glm::mat4& () { return transform.model; }
  operator const glm::mat4& () const { return transform.model; }
};

struct GpuPassComp {
  bool visible = true;
  int order = 0;
  VulkanRenderPassPtr pass;
  VulkanLayoutPtr layout;
};

struct GpuDrawComp {
  bool visible = true;
  int order = 0;
  uint32_t instances = 1;
  entt::entity pass = entt::null;
  VulkanMeshPtr mesh;
  VulkanPipelinePtr pipeline;
  VulkanLayoutSetPtr layoutSet;
  GuiContextPtr guiContext;
};


enum GuiAlignment {
  GuiAlignCenter,
  GuiAlignLeft,
  GuiAlignRight,
  GuiAlignTop,
  GuiAlignBottom,
};

struct GuiBoundsComp {
  glm::vec2 position{0.0f, 0.0f};
  glm::vec2 size{0.0f, 0.0f};
};

struct GuiPanelComp {
  int widgetCount  = 0;
  bool visible     = true;
  bool collapsed   = false;
  bool collapsable = false;
  bool moveable    = false;
  bool sizeable    = false;
  bool background  = false;
  GuiAlignment alignX = GuiAlignLeft;
  GuiAlignment alignY = GuiAlignTop;
  glm::vec2 offset{0.0f, 0.0f};
};

enum GuiWidgetType {
  GuiLabel,
  GuiButton,
};

struct GuiWidgetComp {
  int order = 0;
  GuiWidgetType type = GuiLabel;
  std::string text = "";
};

struct ScriptComp {
  void* instance = nullptr;
  
  std::function<void(EcsEntity&)> createFunc;
  std::function<void(EcsEntity&)> destroyFunc;
  
  std::function<void(EcsEntity&)> onCreateFunc;
  std::function<void(EcsEntity&)> onDestroyFunc;
  std::function<void(EcsEntity&, float)> onUpdateFunc;
  
  template<typename T>
  void bind() {
    createFunc = [&](EcsEntity& entity) {
      if (!instance) {
        instance = (void*) new T();
        onCreateFunc(entity);
      }
    };
    
    destroyFunc = [&](EcsEntity& entity) {
      if (instance) {
        onDestroyFunc(entity);
        delete (T*)instance;
        instance = nullptr;
      }
    };
    
    onCreateFunc = [&](EcsEntity& entity) { ((T*)instance)->onCreate(entity); };
    onDestroyFunc = [&](EcsEntity& entity) { ((T*)instance)->onDestory(entity); };
    onUpdateFunc = [&](EcsEntity& entity, float ts) { ((T*)instance)->onUpdate(entity, ts); };
  }
};

#endif /* EcsComponents_hpp */
