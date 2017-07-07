//
//  Material.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Material_h
#define Material_h

#include "GraphicResources.h"
#include "GraphicStates.h"
#include "TextureMap.h"


namespace fx {
  
  struct MaterialData {
    vec4 ambiant;
    vec4 diffuse;
    vec4 specular;
    vec4 factors;
  };
  
  class Material {
  private:
    MaterialData _data;
    TextureMap _textures;
    ShaderProgram *_shader;
    
  public:
    Material(): _shader(nullptr) {}
    ~Material() {delete _shader;}
    
    void setShader(ShaderProgram *shader) {_shader = shader;}
    ShaderProgram* shader() {return _shader;}
    
    void addTexture(TextureBuffer *texture, SamplerState sampler = SamplerState()) {
      _textures.addTexture(texture, sampler);
    }
    TextureMap& textures() {return _textures;}
    
    MaterialData& data() {return _data;}
    
    void setAmbiant(const vec3 &color, float factor) {_data.ambiant = vec4(color, factor);}
    vec3 ambiantColor() const {return _data.ambiant.xyz();}
    float ambiantFactor() const {return _data.ambiant.a;}
    
    void setDiffuse(const vec3 &color, float factor) {_data.diffuse = vec4(color, factor);}
    vec3 diffuseColor() const {return _data.diffuse.xyz();}
    float diffuseFactor() const {return _data.diffuse.a;}
    
    void setSpecular(const vec3 &color, float factor) {_data.specular = vec4(color, factor);}
    vec3 specularColor() const {return _data.specular.xyz();}
    float specularFactor() const {return _data.specular.a;}
  };
  
}

#endif /* Material_h */
