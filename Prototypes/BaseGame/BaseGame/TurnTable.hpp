//
//  TurnTable.hpp
//  BaseGame
//
//  Created by Robert Crosby on 4/14/26.
//

#ifndef TurnTable_hpp
#define TurnTable_hpp

#include <FelixEngine/Scene.hpp>
#include <FelixEngine/GuiEvent.hpp>
#include <glm/glm.hpp>


class TurnTable {
private:
  glm::mat4 mModel;
  float mRotation = 0.0f;
  bool mRotating = true;
  
  Felix::Entity mStartButton;
  Felix::Entity mPauseButton;
  
  //Felix::Entity mStartMenu;
  //Felix::Entity mPauseMenu;
  
public:
  void onCreate(Felix::Entity& entity);
  void onDestory(Felix::Entity& entity);
  void onUpdate(Felix::Entity& entity, float ts);
  void onGuiEvent(Felix::Entity& entity, const Felix::GuiEvent& event);
};

#endif /* TurnTable_hpp */
