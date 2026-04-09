//
//  EcsEntity.hpp
//  SDLGame
//
//  Created by Robert Crosby on 3/23/26.
//

#ifndef EcsEntity_hpp
#define EcsEntity_hpp

#include <stdio.h>
#include "entt.hpp"
#include "EcsComponents.hpp"
//#include "EcsEvent.hpp"
#include "EcsScene.hpp"

class EcsEvent;

class EcsEntity {
private:
  entt::entity mHandle;
  EcsScene *mScene;
  
public:
  EcsEntity();
  EcsEntity(entt::entity entity, EcsScene* scene);
  EcsEntity(const EcsEntity& other) = default;
  void destory();
  
  entt::entity handle() const { return mHandle; }
  EcsScene& scene() const { return *mScene; }
  
  EcsEntity addChild(const std::string& name = "");
  
  EcsEntity addGuiWidget(const std::string& label, GuiWidgetType type);
  EcsEntity addGuiLabel(const std::string& label) { return addGuiWidget(label, GuiLabel); }
  EcsEntity addGuiButton(const std::string& label) { return addGuiWidget(label, GuiButton); }
  
  operator bool() const { return mScene != nullptr && mHandle != entt::null; }
  
  template<typename T>
  bool has() const {
    return *this && mScene->mRegistry.all_of<T>(mHandle);
  }
  
  template<typename T, typename... Args>
  T& add(Args&&... args) {
    assert(!has<T>());
    return mScene->mRegistry.emplace<T>(mHandle, std::forward<Args>(args)...);
  }
  
  template<typename T>
  T& get() {
    return has<T>() ? mScene->mRegistry.get<T>(mHandle) : add<T>();
  }
  
  template<typename T>
  const T& get() const {
    assert(has<T>());
    return mScene->mRegistry.get<T>(mHandle);
  }
  
  template<typename T>
  void remove() {
    if (has<T>())
      mScene->mRegistry.remove<T>(mHandle);
  }
  
  void disconnect();
  void trigger(EcsEventType event);
  void enqueue(EcsEventType event);
};

#endif /* EcsEntity_hpp */
