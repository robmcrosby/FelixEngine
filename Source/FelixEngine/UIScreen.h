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
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
    }
  };
}

#endif /* UIScreen_h */
