//
//  ViewSlots.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/14/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef ViewSlots_h
#define ViewSlots_h

#include "Component.h"
#include "View.h"


namespace fx
{
  class ViewSlots: public Component
  {
  public:
    ViewSlots(Object *obj);
    virtual ~ViewSlots();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    void update(void*);
    
  private:
    bool addView(const XMLTree::Node &node);
    
    Scene *mScene;
    std::list<View*> mViews;
    GraphicSystem *mGraphicSystem;
  };
}


#endif /* ViewSlots_hpp */
