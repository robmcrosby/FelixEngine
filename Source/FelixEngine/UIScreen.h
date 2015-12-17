//
//  UIScreen.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIScreen_h
#define UIScreen_h

#include "Component.h"


namespace fx
{
  /**
   *
   */
  class UIScreen: public Component
  {
  public:
    UIScreen(Scene *scene): Component("UIScreen", scene) {std::cout << "Created UIScreen" << std::endl;}
    virtual ~UIScreen() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = Component::setToXml(node);
      if (success)
      {
      }
      return success;
    }
  };
}

#endif /* UIScreen_h */
