//
//  Material.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Material_h
#define Material_h

#include "Scene.h"
#include "Graphics.h"
#include "GraphicStates.h"
#include "TextureMap.h"
#include "UniformMap.h"
#include "GraphicStructures.h"


namespace fx {
  DEFINE_OBJ_BUILDER(Material)
  
  class Material: public iObject {
  private:
    static MaterialBuilder materialBuilder;
    
  private:
    Scene *_scene;
    
    STR_Material _properties;
    
    TexturesPtr      _textures;
    ShaderProgramPtr _shader;
    UniformsPtr      _uniforms;
    
    DepthState _depthState;
    BlendState _blendState;
    
  public:
    Material();
    virtual ~Material();
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
    
    virtual void setToTask(GraphicTask &task);
    
    bool setShader(const XMLTree::Node &node);
    void setShader(const std::string &name);
    void setShader(ShaderProgramPtr &shader) {_shader = shader;}
    ShaderProgramPtr shader() {return _shader;}
    
    bool addTexture(const XMLTree::Node &node);
    bool addTextures(const XMLTree::Node &node);
    void addTexture(TextureBufferPtr &texture, SamplerState sampler = SamplerState());
    TexturesPtr textures() {return _textures;}
    
    STR_Material& properties() {return _properties;}
    
    DepthState& depthState() {return _depthState;}
    void enableDepthTesting() {_depthState.enableDefaultTesting();}
    void disableDepthTesting() {_depthState.disable();}
    
    BlendState& blendState() {return _blendState;}
    void enableBlending() {_blendState.enableDefaultBlending();}
    void disableBlending() {_blendState.disable();}
    
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

#endif /* Material_h */
