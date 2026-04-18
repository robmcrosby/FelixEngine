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
    updateFboSize(entity);
    updateProjection(entity);
  }
}

void GameCamera::updateProjection(Entity entity) {
  auto& camera = entity.get<Camera>();
  auto& gpuPass = entity.get<GpuPass>();
  auto extent = gpuPass.renderPass->getExtent();
  
  if (extent.width == 0 || extent.height == 0) {
    updateFboSize(entity);
    extent = gpuPass.renderPass->getExtent();
  }
  
  vec3 aspect(1.0f);
  if (extent.width < extent.height)
    aspect.y = (float)extent.height/(float)extent.width;
  else
    aspect.x = (float)extent.width/(float)extent.height;
  
  //aspect *= 0.5f;
  //camera.projection = glm::ortho(-aspect.x, aspect.x, -aspect.y, aspect.y, -100.0f, 100.0f);
  
  aspect *= 0.04f;
  camera.projection = glm::frustum(-aspect.x, aspect.x, -aspect.y, aspect.y, 0.1f, 100.0f);
  
  camera.view = lookAt(vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
}

void GameCamera::updateFboSize(Felix::Entity entity) {
  auto& mainPass = entity.parent().get<GpuPass>();
  auto& fboPass = entity.get<GpuPass>();
  
  auto extent = mainPass.renderPass->getExtent();
  assert(fboPass.renderPass->resize(extent));
}
