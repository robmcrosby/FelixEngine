//
//  CompositeCamera.hpp
//  BaseGame
//
//  Created by Robert Crosby on 4/16/26.
//

#ifndef CompositeCamera_hpp
#define CompositeCamera_hpp

#include <FelixEngine/Scene.hpp>
#include <SDL3/SDL.h>


class CompositeCamera {
private:
  
public:
  void onCreate(Felix::Entity& entity);
  void onDestory(Felix::Entity& entity);
  void onUpdate(Felix::Entity& entity, float ts);
  void onSdlEvent(Felix::Entity& entity, SDL_Event* event);
  
private:
  void updateProjection(Felix::Entity entity);
};

#endif /* CompositeCamera_hpp */
