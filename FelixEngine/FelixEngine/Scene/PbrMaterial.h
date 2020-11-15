//
//  PbrMaterial.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/15/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef PbrMaterial_h
#define PbrMaterial_h

#include "Material.h"


namespace fx {
  DEFINE_OBJ_BUILDER(PbrMaterial)
  
  class PbrMaterial: public Material {
  private:
    static PbrMaterialBuilder materialBuilder;
    
  private:
    STR_Material _properties;
    
  public:
    PbrMaterial();
    virtual ~PbrMaterial();
    
    virtual bool load(const XMLTree::Node &node);
    virtual bool load(const USDCrate &crate, const std::string &path);
    virtual void update(float dt);
    
    STR_Material& properties() {return _properties;}
    
    void setDiffuse(const vec3 &color, float factor) {_properties.diffuse = vec4(color, factor);}
  };
}

#endif /* PbrMaterial_h */
