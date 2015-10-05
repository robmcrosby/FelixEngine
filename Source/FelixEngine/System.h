//
//  System.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef System_h
#define System_h

#include <istream>

namespace fx
{
  enum SYSTEM_TYPE
  {
    SYSTEM_GPU = 0,
    SYSTEM_AUDIO,
    SYSTEM_PHYSICS,
    SYSTEM_OTHER,
    SYSTEM_COUNT,
  };
  
  /**
   * Defines the base class for a System used in the Felix Engine.
   */
  class System
  {
  public:
    System(SYSTEM_TYPE type = SYSTEM_OTHER);
    virtual ~System();
    
    SYSTEM_TYPE type() const {return mType;}
    
  protected:
    SYSTEM_TYPE mType;
  };
}


#endif /* System_h */
