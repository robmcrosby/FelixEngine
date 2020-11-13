//
//  Material.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Material_h
#define Material_h

#include <FelixEngine/Scene.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/GraphicStates.h>
#include <FelixEngine/TextureMap.h>
#include <FelixEngine/UniformMap.h>
#include <FelixEngine/GraphicStructures.h>


namespace fx {
  class USDCrate;

  DEFINE_OBJ_BUILDER(Material)
  
  class Material: public iObject {
  private:
    static MaterialBuilder materialBuilder;
    
  protected:
    Scene *_scene;
    
    //STR_Material _properties;
    
    TexturesPtr      _textures;
    ShaderProgramPtr _shader;
    UniformsPtr      _uniforms;
    
    DepthState _depthState;
    BlendState _blendState;
    
  public:
    Material();
    virtual ~Material();
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool load(const XMLTree::Node &node);
    virtual bool load(const USDCrate &crate, const std::string &path);
    virtual void update(float dt);
    
    bool setShader(const XMLTree::Node &node);
    void setShader(const std::string &name);
    void setShader(ShaderProgramPtr &shader) {_shader = shader;}
    ShaderProgramPtr shader() {return _shader;}
    bool loadShader(const std::string &vertex, const std::string &fragment);
    
    bool setTexture(const XMLTree::Node &node);
    bool setTextures(const XMLTree::Node &node);
    void setTexture(const std::string &name, TextureBufferPtr &texture, SamplerState sampler = SamplerState());
    TexturesPtr textures() {return _textures;}
    UniformsPtr uniforms() {return _uniforms;}
    
    //STR_Material& properties() {return _properties;}
    
    DepthState& depthState() {return _depthState;}
    void enableDepthTesting() {_depthState.enableDefaultTesting();}
    void disableDepthTesting() {_depthState.disable();}
    
    BlendState& blendState() {return _blendState;}
    void enableBlending() {_blendState.enableDefaultBlending();}
    void disableBlending() {_blendState.disable();}
    
  private:
    bool loadXMLItem(const XMLTree::Node &node);
  };
}

#endif /* Material_h */
