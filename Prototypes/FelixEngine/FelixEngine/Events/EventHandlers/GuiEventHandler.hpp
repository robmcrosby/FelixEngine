//
//  GuiEventHandler.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/11/26.
//

#ifndef GuiEventHandler_hpp
#define GuiEventHandler_hpp

#include <FelixEngine/GuiEvent.hpp>


namespace Felix {

class GuiEventHandler {
private:
  void* mHandler = nullptr;
  Entity mEntity;
  std::function<void(const GuiEvent&)> mOnEventFunc;
  
public:
  void onEvent(const GuiEvent& event) {
    if (mEntity && mHandler && mOnEventFunc)
      mOnEventFunc(event);
  }
  
  template<typename T>
  void connect(Entity entity, T* handler) {
    if (mHandler && mEntity)
      return;
    
    mEntity = entity;
    mHandler = (void*)handler;
    mOnEventFunc = [&](const GuiEvent& event) { ((T*)mHandler)->onGuiEvent(mEntity, event); };
    
    auto& dispatcher = mEntity.scene().dispatcher();
    dispatcher.sink<GuiEvent>().connect<&GuiEventHandler::onEvent>(this);
  }
  
  void disconnect() {
    if (mHandler && mEntity) {
      auto& dispatcher = mEntity.scene().dispatcher();
      dispatcher.sink<GuiEvent>().disconnect(this);
      mHandler = nullptr;
      mEntity = Entity();
    }
  }
};

} /* Felix */

#endif /* GuiEventHandler_hpp */
