//
//  Graphics.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Graphics_h
#define Graphics_h

#include <list>
#include "Component.h"

namespace fx
{
  class GraphicSystem;
  class GraphicResource;
  
  /**
   *
   */
  class Graphics: public Component
  {
  public:
    Graphics(Object *obj);
    virtual ~Graphics();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    void addResource(GraphicResource *resource);
    
  private:
    GraphicSystem *mGraphicSystem;
    std::list<GraphicResource*> mResources;
  };
}

#endif /* Graphics_hpp */
