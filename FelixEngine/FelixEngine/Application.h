//
//  Application.hpp
//  FelixEngine
//
//  Created by Robert Crosby on 6/13/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Application_h
#define Application_h

#include <stdio.h>

namespace fx {
  class Application {
  public:
    Application();
    virtual ~Application();
    
    virtual void start();
  };
}

#endif /* Application_hpp */
