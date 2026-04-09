//
//  EcsEvent.hpp
//  SDLGame
//
//  Created by Robert Crosby on 4/7/26.
//

#ifndef EcsEvent_hpp
#define EcsEvent_hpp

#include "EcsComponents.hpp"
#include "EcsEntity.hpp"


struct EcsEvent {
  EcsEventType type = UndefinedEvent;
  EcsEntity sender;
  
  EcsEvent() = default;
  EcsEvent(EcsEventType t, EcsEntity s): type(t), sender(s) {}
  EcsEvent(const EcsEvent&) = default;
  
  operator EcsEventType() const { return type; }
};

struct ListenerComp {
  void* listener = nullptr;
  std::function<void(const EcsEvent&)> onEventFunc;
  
  void onEvent(const EcsEvent& event) {
    if (listener && onEventFunc)
      onEventFunc(event);
  }
  
  template<typename T>
  void connect(entt::dispatcher& dispatcher, T& item) {
    listener = (void*)&item;
    onEventFunc = [&](const EcsEvent& event) { ((T*)listener)->onEvent(event); };
    dispatcher.sink<EcsEvent>().connect<&ListenerComp::onEvent>(this);
  }
  
  template<typename T>
  void connect(const EcsEntity& entity, T& item) {
    connect(entity.scene().dispatcher(), item);
  }
  
  void disconnect(entt::dispatcher& dispatcher) {
    dispatcher.sink<EcsEvent>().disconnect(this);
    listener = nullptr;
    onEventFunc = nullptr;
  }
  
  void disconnect(const EcsEntity& entity) {
    disconnect(entity.scene().dispatcher());
  }
};


#endif /* EcsEvent_hpp */
