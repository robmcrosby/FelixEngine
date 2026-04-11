//
//  GameCamera.cpp
//  BaseGame
//
//  Created by Robert Crosby on 4/11/26.
//

#include "GameCamera.hpp"
#include <FelixEngine/GpuComponents.hpp>
#include <FelixEngine/SdlEventHandler.hpp>
#include <FelixEngine/VulkanIncludes.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
using namespace glm;
using namespace Felix;

void GameCamera::onCreate(Entity& entity) {
  entity.get<SdlEventHandler>().connect(entity, this);
}

void GameCamera::onDestory(Entity& entity) {
  entity.get<SdlEventHandler>().disconnect();
}

void GameCamera::onUpdate(Entity& entity, float ts) {
  updateProjection(entity);
}

void GameCamera::onSdlEvent(Entity& entity, SDL_Event* event) {
  if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    updateProjection(entity);
  }
}

void GameCamera::updateProjection(Entity entity) {
  auto& camera = entity.get<Camera>();
  auto& gpuPass = entity.get<GpuPass>();
  auto extent = gpuPass.renderPass->getExtent();
  
  vec3 aspect(1.0f);
  if (extent.width < extent.height)
    aspect.x = (float)extent.height/(float)extent.width;
  else
    aspect.y = (float)extent.width/(float)extent.height;
  
  camera.projection = glm::scale(mat4(1.0f), aspect);
}
