//
//  Material.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/12/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Material_h
#define Material_h

#include "FelixEngine.h"
#include "Scene.h"
#include "Buffer.h"
#include "OwnPtr.h"
#include "XMLTree.h"


namespace fx
{
  class Material
  {
  public:
    Material(Scene *scene): mScene(scene) {}
    virtual ~Material() {}
    
    void setToXml(const XMLTree::Node &node)
    {
    }
    
  private:
    Scene *mScene;
  };
}

#endif /* Material_h */
