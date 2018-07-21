//
//  LightRig.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/21/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef LightRig_h
#define LightRig_h

#include "Drawable.h"
#include "Graphics.h"
#include "UniformMap.h"
#include "Transform.h"
#include "GraphicStructures.h"


namespace fx {
  DEFINE_OBJ_BUILDER(LightRig)
  
  class LightRig: public Drawable {
  private:
    static LightRigBuilder lightRigBuilder;
    
  protected:
    
  public:
    LightRig();
    virtual ~LightRig();
    
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
  };
}


#endif /* LightRig_h */
