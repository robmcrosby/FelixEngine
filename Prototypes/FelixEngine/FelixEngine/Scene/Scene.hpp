//
//  Scene.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <FelixEngine/Binding.hpp>
#include <FelixEngine/GuiEventTypes.hpp>
#include <SDL3/SDL.h>
#include <iostream>
#include <entt.hpp>


namespace Felix {

class Entity;

class Scene {
private:
  entt::registry mRegistry;
  entt::dispatcher mDispatcher;
  uint64_t mStartTime = 0;
  uint64_t mLastTime = 0;
  bool mPaused = true;
  
public:
  Scene() = default;
  
  bool isPaused() const { return mPaused; }
  entt::registry& registry() { return mRegistry; }
  entt::dispatcher& dispatcher() { return mDispatcher; }
  
  Entity add(const std::string& name = "");
  Entity get(const std::string& name);
  
  void resume();
  void pause();
  void destory();
  void onUpdate();
  
  void handle(SDL_Event* event);
  void updateEvents();
  
  template<typename T>
  void sort() {
    mRegistry.sort<T>([](const T& lhs, const T& rhs) { return lhs < rhs; });
  }
  
private:
  friend Entity;
};


class Entity {
private:
  entt::entity mEntity;
  Scene *mScene;
  
public:
  Entity();
  Entity(entt::entity entity, Scene* scene);
  Entity(const Entity& other) = default;
  
  void destory();
  
  entt::entity handle() const { return mEntity; }
  Scene& scene() const { return *mScene; }
  
  operator bool() const { return mScene != nullptr && mEntity != entt::null; }
  
  Entity addChild(const std::string& name = "");
  Entity addChild(Entity child);
  Entity parent() const;
  
  uint64_t uuid() const;
  
  std::string& tag();
  const std::string& tag() const;
  
  void enqueue(GuiEventType event);
  
  bool operator==(const Entity& other) const {
    return mScene && mScene == other.mScene && mEntity == other.mEntity;
  }
  
  bool operator==(entt::entity entity) const {
    return mScene && mEntity == entity;
  }
  
  template<typename T>
  bool has() const {
    return *this && mScene->mRegistry.all_of<T>(mEntity);
  }
  
  template<typename T, typename... Args>
  T& add(Args&&... args) {
    assert(!has<T>());
    return mScene->mRegistry.emplace<T>(mEntity, std::forward<Args>(args)...);
  }
  
  template<typename T>
  T& get() {
    return has<T>() ? mScene->mRegistry.get<T>(mEntity) : add<T>();
  }
  
  template<typename T>
  const T& get() const {
    assert(has<T>());
    return mScene->mRegistry.get<T>(mEntity);
  }
  
  template<typename T>
  void remove() {
    if (has<T>())
      mScene->mRegistry.remove<T>(mEntity);
  }
  
  template<typename T>
  void bind() {
    get<Binding>().bind<T>();
  }
};

} /* Felix */

#endif /* Scene_hpp */
