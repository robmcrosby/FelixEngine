//
//  Binding.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 4/10/26.
//

#ifndef Binding_hpp
#define Binding_hpp

#include <iostream>


namespace Felix {

class Entity;
class Binding {
private:
  void* mInstance = nullptr;
  
  std::function<void(Entity&)> mCreateFunc;
  std::function<void(Entity&)> mDestroyFunc;
  
  std::function<void(Entity&)> mOnCreateFunc;
  std::function<void(Entity&)> mOnDestroyFunc;
  std::function<void(Entity&, float)> mOnUpdateFunc;
  
public:
  void onStart(Entity& entity) {
    if (!mInstance && mCreateFunc)
      mCreateFunc(entity);
  }
  
  void onDestory(Entity& entity) {
    if (mInstance && mDestroyFunc)
      mDestroyFunc(entity);
  }
  
  void onUpdate(Entity& entity, float ts) {
    if (mInstance && mOnUpdateFunc)
      mOnUpdateFunc(entity, ts);
  }
  
  template<typename T>
  void bind() {
    if (mInstance != nullptr)
      return;
    
    mCreateFunc = [&](Entity& entity) {
      if (!mInstance) {
        mInstance = (void*) new T();
        mOnCreateFunc(entity);
      }
    };
    
    mDestroyFunc = [&](Entity& entity) {
      if (mInstance) {
        mOnDestroyFunc(entity);
        delete (T*)mInstance;
        mInstance = nullptr;
      }
    };
    
    mOnCreateFunc = [&](Entity& entity) { ((T*)mInstance)->onCreate(entity); };
    mOnDestroyFunc = [&](Entity& entity) { ((T*)mInstance)->onDestory(entity); };
    mOnUpdateFunc = [&](Entity& entity, float ts) { ((T*)mInstance)->onUpdate(entity, ts); };
  }
};

} /* Felix */

#endif /* Binding_hpp */
