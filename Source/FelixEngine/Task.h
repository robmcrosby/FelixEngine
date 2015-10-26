//
//  Task.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Task_h
#define Task_h

#include <iostream>

template <typename RetType, typename ParamType>
class Task
{
public:
  Task(): objPtr(0), stubPtr(0) {}
  
  template <class T, RetType (T::*TMethod)(ParamType)>
  static Task fromMethod(T *objPtr)
  {
    Task d;
    d.objPtr = objPtr;
    d.stubPtr = &MethodStub<T, TMethod>;
    return d;
  }
  RetType operator()(int a1) const {return (*stubPtr)(objPtr, a1);}
  
private:
  typedef RetType (*StubType)(void *objPtr, ParamType);
  
  void     *objPtr;
  StubType stubPtr;
  
  template <class T, RetType (T::*TMethod)(int)>
  static RetType MethodStub(void *objPtr, ParamType a1)
  {
    T *p = static_cast<T*>(objPtr);
    return (p->*TMethod)(a1);
  }
};

#endif /* Task_h */
