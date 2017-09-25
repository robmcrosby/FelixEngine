//
//  Light.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/23/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Light_h
#define Light_h

#include "Scene.h"
#include "GraphicTask.h"

namespace fx {
  class Light;
  typedef std::shared_ptr<Light> LightPtr;
  
  struct LightPeramaters {
    vec4 position;
    vec4 direction;
    vec4 color;
    vec4 factors;
  };
  
  class Light: public iObject {
  private:
    Scene *_scene;
    
    LightPeramaters _peramaters;
    
  public:
    Light();
    virtual ~Light();
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
    
    virtual void applyToTask(GraphicTask &task);
    
    void setPosition(vec3 pos) {
      _peramaters.position = vec4(pos, _peramaters.position.w);
    }
    vec3 position() const {return _peramaters.position.xyz();}
    
    void setDirection(vec3 dir) {
      _peramaters.direction = vec4(dir.normalized(), _peramaters.direction.w);
    }
    vec3 direction() const {return _peramaters.direction.xyz();}
    
    void setColor(vec3 color) {_peramaters.color = vec4(color, energy());}
    vec3 color() const {return _peramaters.color.xyz();}
    
    void setEnergy(float energy) {_peramaters.color.w = energy;}
    float energy() const {return _peramaters.color.w;}
    
    void setLinearAttenuation(float attenuation) {_peramaters.factors.x = attenuation;}
    float linearAttenuation() const {return _peramaters.factors.x;}
    
    void setSquareAttenuation(float attenuation) {_peramaters.factors.y = attenuation;}
    float squareAttenuation() const {return _peramaters.factors.y;}
    
    void setDistance(float dist) {_peramaters.direction.w = dist;}
    float distance() const {return _peramaters.direction.w;}
    
    void setConeAngle(float angle) {_peramaters.factors.z = angle;}
    float coneAngle() const {return _peramaters.factors.z;}
    
    void setSoftAngle(float angle) {_peramaters.factors.w = angle;}
    float softAngle() const {return _peramaters.factors.w;}
    
    void setAsDirectionalLight() {_peramaters.position.w = 0.0f;}
    void setAsDirectionalLight(vec3 direction, vec3 color, float energy) {
      setAsDirectionalLight();
      setDirection(direction);
      setColor(color);
      setEnergy(energy);
    }
    bool isDirectionalLight() const {return _peramaters.position.w == 0.0f;}
    
    void setAsPointLight() {_peramaters.position.w = 1.0f;}
    void setAsPointLight(vec3 position, vec3 color, float energy) {
      setAsPointLight();
      setPosition(position);
      setColor(color);
      setEnergy(energy);
    }
    bool isPointLight() const {return _peramaters.position.w == 1.0f;}

  };
}

#endif /* Light_h */
