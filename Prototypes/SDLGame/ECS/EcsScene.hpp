//
//  EcsScene.hpp
//  SDLGame
//
//  Created by Robert Crosby on 3/23/26.
//

#ifndef EcsScene_hpp
#define EcsScene_hpp

#include "entt.hpp"
#include <glm/glm.hpp>

class EcsEntity;
class EcsEvent;
class GuiContext;


class EcsScene {
private:
  entt::registry mRegistry;
  entt::dispatcher mDispatcher;
  
public:
  EcsScene();
  ~EcsScene();
  
  entt::registry& registry() { return mRegistry; }
  entt::dispatcher& dispatcher() { return mDispatcher; }
  
  EcsEntity create(const std::string& name = "");
  EcsEntity createGuiPanel(const std::string& name);
  void remove(EcsEntity entity);
  
  void sortGui();
  
  void onUpdate(uint64_t currentTime, uint64_t elapsedTime);
  void onWindowResize(const glm::vec2& size);
  
  void trigger(const EcsEvent& event);
  void enqueue(const EcsEvent& event);
  void updateEvents();
  
private:
  friend EcsEntity;
};


#endif /* EcsScene_hpp */
