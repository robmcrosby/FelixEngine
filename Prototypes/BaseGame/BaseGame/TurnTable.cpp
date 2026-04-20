//
//  TurnTable.cpp
//  BaseGame
//
//  Created by Robert Crosby on 4/14/26.
//

#include "TurnTable.hpp"
#include <FelixEngine/GuiEventHandler.hpp>
#include <FelixEngine/GpuComponents.hpp>
#include <FelixEngine/GuiPanel.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;
using namespace Felix;


void TurnTable::onCreate(Felix::Entity& entity) {
  //mModel = entity.get<Transform>().model;
  mModel = entity.get<Transforms>().at(0).model;
  mRotation = 0.0f;
  
  mStartButton = entity.scene().get("button1");
  mPauseButton = entity.scene().get("button3");

  entity.get<GuiEventHandler>().connect(entity, this);
  mStartButton.parent().get<GuiPanel>().visible = !mRotating;
  mPauseButton.parent().get<GuiPanel>().visible = mRotating;
}

void TurnTable::onDestory(Felix::Entity& entity) {
  entity.get<GuiEventHandler>().disconnect();
}

void TurnTable::onUpdate(Felix::Entity& entity, float ts) {
  if (!mRotating)
    return;
  
  float twoPi = 2.0f * pi<float>();
  float rotation = mRotation + 0.1f * twoPi * ts;
  mRotation = fmod(rotation, twoPi);
  
  mat4 model = glm::rotate(mModel, mRotation, vec3(0.0f, 1.0f, 0.0f));
  //entity.get<Transform>().model = model;
  auto& transforms = entity.get<Transforms>();
  transforms.at(0).model = glm::translate(model, vec3(0.5f, 0.0f, 0.0f));
  transforms.at(1).model = glm::translate(model, vec3(-0.5f, 0.0f, 0.0f));
  
  entity.get<GpuDraw>().instances = 2;
}

void TurnTable::onGuiEvent(Felix::Entity& entity, const Felix::GuiEvent& event) {
  if (event == ButtonPressEvent) {
    if (event.sender == mPauseButton) {
      mRotating = false;
      mStartButton.parent().get<GuiPanel>().visible = !mRotating;
      mPauseButton.parent().get<GuiPanel>().visible = mRotating;
    }
    else if (event.sender == mStartButton) {
      mRotating = true;
      mStartButton.parent().get<GuiPanel>().visible = !mRotating;
      mPauseButton.parent().get<GuiPanel>().visible = mRotating;
    }
  }
}
