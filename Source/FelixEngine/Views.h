//
//  Views.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Views_h
#define Views_h

#include "Component.h"

namespace fx
{
  /**
   *
   */
  class Views: public Component
  {
  public:
    Views();
    virtual ~Views();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
  };
}

#endif /* Views_h */
