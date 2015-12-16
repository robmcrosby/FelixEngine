//
//  UIWidget.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UIWidget_h
#define UIWidget_h

#include "Object.h"


namespace fx
{
  /**
   *
   */
  class UIWidget: public Object
  {
  public:
    UIWidget(Scene *scene): Object("UIWidget", scene) {std::cout << "Created UIWidget" << std::endl;}
    virtual ~UIWidget() {}
    
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

#endif /* UIWidget_h */
