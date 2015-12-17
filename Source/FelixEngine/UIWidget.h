//
//  UIWidget.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIWidget_h
#define UIWidget_h

#include "Component.h"


namespace fx
{
  /**
   *
   */
  class UIWidget: public Component
  {
  public:
    UIWidget(Scene *scene): Component("UIWidget", scene) {std::cout << "Created UIWidget" << std::endl;}
    virtual ~UIWidget() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
    }
  };
}

#endif /* UIWidget_h */
