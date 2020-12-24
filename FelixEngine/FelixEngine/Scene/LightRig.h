//
//  LightRig.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/21/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef LightRig_h
#define LightRig_h

#include <FelixEngine/Drawable.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/UniformMap.h>
#include <FelixEngine/Transform.h>
#include <FelixEngine/GraphicStructures.h>
#include <FelixEngine/Light.h>
#include <FelixEngine/TextureMap.h>


namespace fx {
  DEFINE_OBJ_BUILDER(LightRig)
  
  class LightRig: public Drawable {
  private:
    static LightRigBuilder lightRigBuilder;
    
  protected:
    std::vector<LightPtr> _lights;
    Texture _environmentMap;
    std::map<std::string, ShaderProgramPtr> _shaderMap;
    
  public:
    LightRig();
    virtual ~LightRig();
    
    virtual bool load(const XMLTree::Node &node);
    virtual void update(float dt);
    
    bool loadXMLItem(const XMLTree::Node &node);
    
    bool setEnvironmentMap(const XMLTree::Node &node);
    
    bool addShader(const XMLTree::Node &node);
    
    bool addLight(const XMLTree::Node &node);
    void addDirectionalLight(vec3 direction, vec3 color, float energy);
    void addPointLight(vec3 location, vec3 color, float energy);
    void addLight(LightPtr light);
    LightPtr getLight(int index);
    void clearLights();
  };
}


#endif /* LightRig_h */
