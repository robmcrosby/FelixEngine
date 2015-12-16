//
//  UIScreen.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIScreen_h
#define UIScreen_h

#include "Object.h"


namespace fx
{
  /**
   *
   */
  class UIScreen: public Object
  {
  public:
    UIScreen(Scene *scene): Object("UIScreen", scene) {std::cout << "Created UIScreen" << std::endl;}
    virtual ~UIScreen() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = Object::setToXml(node);
      if (success)
      {
      }
      return success;
    }
  };
}

#endif /* UIScreen_h */
