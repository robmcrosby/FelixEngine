//
//  Delegate.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Delegate_h
#define Delegate_h

#include <iostream>


namespace fx
{
  template <typename RetType, typename ParamType>
  class Delegate
  {
  public:
    Delegate(): objPtr(0), stubPtr(0) {}
    
    template <class T, RetType(T::*TMethod)(ParamType)>
    static Delegate fromMethod(T *objPtr)
    {
      Delegate d;
      d.objPtr = objPtr;
      d.stubPtr = &MethodStub<T, TMethod>;
      return d;
    }
    RetType operator()(ParamType a1) const {return (*stubPtr)(objPtr, a1);}
    
  private:
    typedef RetType (*StubType)(void *objPtr, ParamType);
    
    void     *objPtr;
    StubType stubPtr;
    
    template <class T, RetType (T::*TMethod)(ParamType)>
    static RetType MethodStub(void *objPtr, ParamType a1)
    {
      T *p = static_cast<T*>(objPtr);
      return (p->*TMethod)(a1);
    }
  };
}

#endif /* Delegate_h */
