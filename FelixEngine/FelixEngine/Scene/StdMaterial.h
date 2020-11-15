//
//  StdMaterial.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/11/20.
//  Copyright © 2020 Robert Crosby. All rights reserved.
//

#ifndef StdMaterial_h
#define StdMaterial_h

#include "Material.h"


namespace fx {
  DEFINE_OBJ_BUILDER(StdMaterial)
  
  class StdMaterial: public Material {
  private:
    static StdMaterialBuilder materialBuilder;
    
  private:
    STR_Material _properties;
    
  public:
    StdMaterial();
    virtual ~StdMaterial();
    
    virtual bool load(const XMLTree::Node &node);
    virtual bool load(const USDCrate &crate, const std::string &path);
    virtual void update(float dt);
    
    STR_Material& properties() {return _properties;}
    
    bool setAmbiant(const XMLTree::Node &node);
    void setAmbiant(const vec3 &color, float factor) {_properties.ambiant = vec4(color, factor);}
    vec3 ambiantColor() const {return _properties.ambiant.xyz();}
    float ambiantFactor() const {return _properties.ambiant.a;}
    
    bool setDiffuse(const XMLTree::Node &node);
    void setDiffuse(const vec3 &color, float factor) {_properties.diffuse = vec4(color, factor);}
    void setDiffuseSize(float size) {_properties.factors.z = size;}
    void setDiffuseSmooth(float smooth) {_properties.factors.w = smooth;}
    void setDiffuseRoughness(float roughness) {_properties.factors.z = roughness;}
    void setDiffuseAlbedo(float albedo) {_properties.factors.w = albedo;}
    vec3 diffuseColor() const {return _properties.diffuse.xyz();}
    float diffuseFactor() const {return _properties.diffuse.a;}
    float diffuseSize() const {return _properties.factors.z;}
    float diffuseSmooth() const {return _properties.factors.w;}
    float diffuseRoughness() const {return _properties.factors.z;}
    float diffuseAlbedo() const {return _properties.factors.w;}
    
    bool setSpecular(const XMLTree::Node &node);
    void setSpecular(const vec3 &color, float factor);
    void setSpecularHardness(float hardness) {_properties.factors.x = hardness;}
    void setSpecularSize(float size) {_properties.factors.x = size;}
    void setSpecularSmooth(float smooth) {_properties.factors.y = smooth;}
    vec3 specularColor() const {return _properties.specular.xyz();}
    float specularFactor() const {return _properties.specular.a;}
    float specularHardness() const {return _properties.factors.x;}
    float specularSize() const {return _properties.factors.x;}
    float specularSmooth() const {return _properties.factors.y;}
    
  private:
    bool loadXMLItem(const XMLTree::Node &node);
  };
}

#endif /* StdMaterial_h */
