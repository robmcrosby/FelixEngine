//
//  Graphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Graphics_h
#define Graphics_h

#include <stdio.h>

namespace fx {
  
  /** File System */
  class Graphics {
  protected:
    static Graphics *instance;
    
  public:
    virtual ~Graphics() {}
  };
  
}

#endif /* Graphics_h */
