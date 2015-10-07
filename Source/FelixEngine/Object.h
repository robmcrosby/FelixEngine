//
//  Object.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/7/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include <list>
#include "Component.h"


namespace fx
{
  /**
   *
   */
  class Object
  {
  public:
    Object() {}
    ~Object() {}
    
  private:
    std::list<Component*> mComponents;
  };
}

#endif /* Object_hpp */
