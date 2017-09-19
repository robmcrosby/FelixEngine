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
  class Scene;
  
  struct MaterialData {
    vec4 ambiant;
    vec4 diffuse;
    vec4 specular;
    vec4 factors;
  };
  
  class Material {
  private:
    Scene *_scene;
    
    MaterialData _data;
    TextureMap _textures;
    ShaderPtr  _shader;
    
    DepthState _depthState;
    BlendState _blendState;
    
  public:
    Material(Scene *scene);
    ~Material();
    
    bool loadXML(const XMLTree::Node &node);
    
    void setShader(const std::string &name);
    void setShader(ShaderPtr &shader) {_shader = shader;}
    ShaderPtr shader() {return _shader;}
    
    void addTexture(TexturePtr &texture, SamplerState sampler = SamplerState());
    TextureMap& textures() {return _textures;}
    
    MaterialData& data() {return _data;}
    
    DepthState& depthState() {return _depthState;}
    void enableDepthTesting() {_depthState.enableDefaultTesting();}
    void disableDepthTesting() {_depthState.disable();}
    
    BlendState& blendState() {return _blendState;}
    void enableBlending() {_blendState.enableDefaultBlending();}
    void disableBlending() {_blendState.disable();}
    
    bool setAmbiant(const XMLTree::Node &node);
    void setAmbiant(const vec3 &color, float factor) {_data.ambiant = vec4(color, factor);}
    vec3 ambiantColor() const {return _data.ambiant.xyz();}
    float ambiantFactor() const {return _data.ambiant.a;}
    
    bool setDiffuse(const XMLTree::Node &node);
    void setDiffuse(const vec3 &color, float factor) {_data.diffuse = vec4(color, factor);}
    vec3 diffuseColor() const {return _data.diffuse.xyz();}
    float diffuseFactor() const {return _data.diffuse.a;}
    
    bool setSpecular(const XMLTree::Node &node);
    void setSpecular(const vec3 &color, float factor, float hardness);
    vec3 specularColor() const {return _data.specular.xyz();}
    float specularFactor() const {return _data.specular.a;}
    float hardness() const {return _data.factors.x;}
  };
  
}

#endif /* Material_h */
