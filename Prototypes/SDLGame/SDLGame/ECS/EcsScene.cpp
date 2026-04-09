//
//  EcsScene.cpp
//  SDLGame
//
//  Created by Robert Crosby on 3/23/26.
//

#include "EcsScene.hpp"
#include "EcsEntity.hpp"
#include "EcsEvent.hpp"
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;

EcsScene::EcsScene() {
  
}

EcsScene::~EcsScene() {
  
}

EcsEntity EcsScene::create(const string& name) {
  EcsEntity entity = {mRegistry.create(), this};
  entity.add<IdComp>();
  entity.add<TagComp>(name);
  return entity;
}

EcsEntity EcsScene::createGuiPanel(const string &name) {
  EcsEntity entity = create(name);
  entity.add<GuiPanelComp>();
  entity.add<GuiBoundsComp>();
  return entity;
}

void EcsScene::sortGui() {
  // Sort Widgets
  mRegistry.sort<GuiWidgetComp>([](const auto &lhs, const auto &rhs) {
    return lhs.order < rhs.order;
  });
  // Sort Drawables
  mRegistry.sort<GpuDrawComp>([](const auto &lhs, const auto &rhs) {
    return lhs.order < rhs.order;
  });
  // Sort RenderPasses
  mRegistry.sort<GpuPassComp>([](const auto &lhs, const auto &rhs) {
    return lhs.order < rhs.order;
  });
}

void EcsScene::onUpdate(uint64_t currentTime, uint64_t elapsedTime) {
  {
    float ts = elapsedTime/1000.0f;
    mRegistry.view<ScriptComp>().each([&](auto entity, auto& scriptComp) {
      EcsEntity ecsEntity(entity, this);
      if (!scriptComp.instance) {
        scriptComp.createFunc(ecsEntity);
      }
      if (scriptComp.instance) {
        scriptComp.onUpdateFunc(ecsEntity, ts);
      }
    });
  }
  
  //printf("elapsedTime: %lld, currentTime %lld\n", elapsedTime, currentTime);
}

void EcsScene::onWindowResize(const glm::vec2& size) {
  //printf("window resized to: %d, %d", width, height);
  vec3 aspect(1.0f);
  if (size.x < size.y)
    aspect.x = (float)size.y/(float)size.x;
  else
    aspect.y = (float)size.x/(float)size.y;
  
  // Update Cameras
  auto cameras = mRegistry.view<CameraComp>();
  for (auto [camera, cameraComp] : cameras.each()) {
    cameraComp.camera.projection = glm::scale(mat4(1.0f), aspect);
  }
}

void EcsScene::trigger(const EcsEvent& event) {
  mDispatcher.trigger<EcsEvent>(event);
}

void EcsScene::enqueue(const EcsEvent& event) {
  mDispatcher.enqueue<EcsEvent>(event);
}

void EcsScene::updateEvents() {
  mDispatcher.update();
}
