//
//  Mesh.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/10/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Mesh_h
#define Mesh_h

#include "Scene.h"
#include "Graphics.h"

namespace fx {
  DEFINE_OBJ_BUILDER(Mesh)
  
  class Mesh: public iObject {
  private:
    static MeshBuilder meshBuilder;
    
  protected:
    VertexPtr _mesh;
    
  public:
    Mesh();
    virtual ~Mesh();
    
    virtual void setScene(Scene *scene) {}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt) {}
  };
}

#endif /* Mesh_h */
