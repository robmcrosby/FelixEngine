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
#include "RenderSlots.h"
#include "Projection.h"
#include "View.h"


namespace fx
{
  /**
   *
   */
  class UIScreen: public Component
  {
  public:
    UIScreen(Scene *scene): Component(scene) {std::cout << "Created UIScreen" << std::endl;}
    virtual ~UIScreen() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      addChildren(node);
      mRenderSlots = getChild<RenderSlots>();
      mProjection = getChild<Projection>();
      mView = getChild<View>();
    }
    
  protected:
    RenderSlots *mRenderSlots;
    Projection *mProjection;
    View *mView;
  };
}

#endif /* UIScreen_h */
