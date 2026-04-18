//
//  GameCamera.hpp
//  BaseGame
//
//  Created by Robert Crosby on 4/11/26.
//

#ifndef GameCamera_hpp
#define GameCamera_hpp

#include <FelixEngine/Scene.hpp>
#include <SDL3/SDL.h>


class GameCamera {
private:
  
public:
  void onCreate(Felix::Entity& entity);
  void onDestory(Felix::Entity& entity);
  void onUpdate(Felix::Entity& entity, float ts);
  void onSdlEvent(Felix::Entity& entity, SDL_Event* event);
  
private:
  void updateProjection(Felix::Entity entity);
  void updateFboSize(Felix::Entity entity);
};

#endif /* GameCamera_hpp */
