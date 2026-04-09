//
//  EcsEntity.cpp
//  SDLGame
//
//  Created by Robert Crosby on 3/23/26.
//

#include "EcsEntity.hpp"
#include "EcsEvent.hpp"


using namespace std;

EcsEntity::EcsEntity(): mHandle(entt::null), mScene(nullptr) {
  
}

EcsEntity::EcsEntity(entt::entity entity, EcsScene* scene): mHandle(entity), mScene(scene) {
  
}

void EcsEntity::destory() {
  mScene->mRegistry.destroy(mHandle);
  mHandle = entt::null;
  mScene = nullptr;
}

EcsEntity EcsEntity::addChild(const string& name) {
  EcsEntity child = mScene->create(name);
  child.get<ParentComp>().parent = mHandle;
  return child;
}

EcsEntity EcsEntity::addGuiWidget(const std::string& label, GuiWidgetType type) {
  int order = 0;
  if (this->has<GuiPanelComp>())
    order = this->get<GuiPanelComp>().widgetCount++;
  
  EcsEntity widget = addChild(label);
  auto& widgetComp = widget.get<GuiWidgetComp>();
  widgetComp.order = order;
  widgetComp.type = type;
  widgetComp.text = label;
  return widget;
}

void EcsEntity::disconnect() {
  get<ListenerComp>().disconnect(mScene->mDispatcher);
}

void EcsEntity::trigger(EcsEventType event) {
  mScene->trigger(EcsEvent(event, *this));
}

void EcsEntity::enqueue(EcsEventType event) {
  mScene->enqueue(EcsEvent(event, *this));
}
