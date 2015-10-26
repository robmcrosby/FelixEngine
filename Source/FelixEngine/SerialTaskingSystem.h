//
//  SerialTaskingSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/26/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef SerialTaskingSystem_h
#define SerialTaskingSystem_h

#include "TaskingSystem.h"


namespace fx
{
  class SerialTaskingSystem: public TaskingSystem
  {
  public:
    SerialTaskingSystem();
    virtual ~SerialTaskingSystem();
    
    virtual void update();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* SerialTaskingSystem_h */
