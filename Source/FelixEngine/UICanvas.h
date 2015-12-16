//
//  UICanvas.h
//  FelixEngine
//
//  Created by Robert Crosby on 12/15/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef UICanvas_h
#define UICanvas_h

#include "Object.h"


namespace fx
{
  /**
   *
   */
  class UICanvas: public Object
  {
  public:
    UICanvas(Scene *scene): Object("UICanvas", scene) {std::cout << "Created UICanvas" << std::endl;}
    virtual ~UICanvas() {}
    
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

#endif /* UICanvas_h */
