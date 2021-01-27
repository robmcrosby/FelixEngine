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
    static TextureBufferPtr   spbrdfLut;

  public:
    PbrMaterial();
    virtual ~PbrMaterial();
    
    virtual bool load(const XMLTree::Node &node);
    virtual bool load(const USDItem &item);
    virtual void update(float dt);
    
  private:
    void addTexture(const std::string &name, const USDItem *input, RGBA def);
    void addTexture(const std::string &name, const USDItem *input, RGBA *def = NULL);
  };
}

#endif /* PbrMaterial_h */
