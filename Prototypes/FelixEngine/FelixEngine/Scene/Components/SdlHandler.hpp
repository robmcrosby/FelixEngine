//
//  SdlHandler.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef SdlHandler_hpp
#define SdlHandler_hpp

#include <FelixEngine/SdlEvent.hpp>
#include <FelixEngine/Scene.hpp>

namespace Felix {

class SdlHandler {
private:
  void* mHandler = nullptr;
  Entity mEntity;
  
  std::function<void(const SdlEvent&)> mOnEventFunc;
public:
  void onEvent(const SdlEvent& event) {
    if (mEntity && mHandler && mOnEventFunc)
      mOnEventFunc(event);
  }
  
  template<typename T>
  void connect(Entity entity, T* handler) {
    if (mHandler && mEntity)
      return;
    
    mEntity = entity;
    mHandler = (void*)handler;
    mOnEventFunc = [&](const SdlEvent& event) { ((T*)mHandler)->onSdlEvent(mEntity, (SDL_Event*)event); };
    
    auto& dispatcher = mEntity.scene().dispatcher();
    dispatcher.sink<SdlEvent>().connect<&SdlHandler::onEvent>(this);
  }
  
  void disconnect() {
    if (mHandler && mEntity) {
      auto& dispatcher = mEntity.scene().dispatcher();
      dispatcher.sink<SdlEvent>().disconnect(this);
      mHandler = nullptr;
      mEntity = Entity();
    }
  }
};

} /* Felix */

#endif /* SdlHandler_hpp */
