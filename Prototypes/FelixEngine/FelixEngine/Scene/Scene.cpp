//
//  Scene.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/9/26.
//

#include "Scene.hpp"
#include "UUID.hpp"
#include "Tag.hpp"
#include "Parent.hpp"
#include "SdlEvent.hpp"


using namespace std;
namespace Felix {

Entity Scene::add(const string& name) {
  Entity entity = {mRegistry.create(), this};
  entity.add<UUID>();
  entity.add<Tag>(name);
  return entity;
}

void Scene::resume() {
  mStartTime = mLastTime = SDL_GetTicks();
  mPaused = false;
  
  mRegistry.view<Binding>().each([&](auto item, auto& binding) {
    Entity entity(item, this);
    binding.onStart(entity);
  });
}

void Scene::pause() {
  mPaused = true;
}

void Scene::destory() {
  pause();
  
  mRegistry.view<Binding>().each([&](auto item, auto& binding) {
    Entity entity(item, this);
    binding.onDestory(entity);
  });
}

void Scene::onUpdate() {
  if (mPaused)
    return;
  
  uint64_t currentTime = SDL_GetTicks();
  uint64_t elapsedTime = currentTime - mLastTime;
  mLastTime = currentTime;
  
  float ts = elapsedTime/1000.0f;
  mRegistry.view<Binding>().each([&](auto item, auto& binding) {
    Entity entity(item, this);
    binding.onUpdate(entity, ts);
  });
}

void Scene::handle(SDL_Event* event) {
  mDispatcher.trigger<SdlEvent>(event);
}

void Scene::updateEvents() {
  mDispatcher.update();
}


Entity::Entity(): mEntity(entt::null), mScene(nullptr) {
  
}

Entity::Entity(entt::entity entity, Scene* scene): mEntity(entity), mScene(scene) {
  
}

void Entity::destory() {
  if (*this)
    mScene->mRegistry.destroy(mEntity);
  mEntity = entt::null;
  mScene = nullptr;
}

Entity Entity::addChild(const std::string& name) {
  return addChild(mScene->add(name));
}

Entity Entity::addChild(Entity child) {
  child.get<Parent>().parent = *this;
  return child;
}

Entity Entity::parent() const {
  return has<Parent>() ? get<Parent>().parent : Entity();
}

uint64_t Entity::uuid() const {
  return *this ? (uint64_t)get<UUID>() : 0;
}

string& Entity::tag() {
  return get<Tag>().tag;
}

const string& Entity::tag() const {
  return get<Tag>().tag;
}

} /* Felix */
