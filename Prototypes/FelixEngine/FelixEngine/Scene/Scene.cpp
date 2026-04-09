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


using namespace std;
namespace Felix {

Entity Scene::add(const string& name) {
  Entity entity = {mRegistry.create(), this};
  entity.add<UUID>();
  entity.add<Tag>(name);
  return entity;
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

} /* Felix */
