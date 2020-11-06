//
//  Light.h
//  FelixEngine
//
//  Created by Robert Crosby on 9/23/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Light_h
#define Light_h

#include <FelixEngine/Drawable.h>
#include <FelixEngine/Transform.h>
#include <FelixEngine/GraphicStructures.h>

namespace fx {
  DEFINE_OBJ_BUILDER(Light)
  
  class Light: public RenderItem, public Drawable {
  private:
    static LightBuilder lightBuilder;
    
  private:
    TransformPtr _transform;
    
    vec3 _color;
    float _energy;
    float _distance;
    
    float _linearAttenuation;
    float _squareAttenuation;
    
    float _coneAngle;
    float _softAngle;
    
    bool _directional;
    
  private:
    bool loadXMLItem(const XMLTree::Node &node);
    
  public:
    Light();
    virtual ~Light();
    
    virtual bool load(const XMLTree::Node &node);
    virtual void update(float dt);
    
    STR_Light peramaters() const {
      STR_Light peramaters;
      peramaters.position = vec4(globalLocation(), _directional ? 0.0f : 1.0f);
      peramaters.direction = vec4(globalDirection(), distance());
      peramaters.color = vec4(color(), energy());
      peramaters.factors = vec4(linearAttenuation(), squareAttenuation(), coneAngle(), softAngle());
      return peramaters;
    }
    
    void setLocation(vec3 location) {_transform->setLocation(location);}
    vec3 localLocation() const {return _transform->localLocation();}
    vec3 globalLocation() const {return _transform->globalLocation();}
    
    void setRotation(quat rotation) {_transform->setRotation(rotation);}
    quat localRotation() const {return _transform->localRotation();}
    quat globalRotation() const {return _transform->globalRotation();}
    
    void setDirection(vec3 dir) {
      _transform->setRotation(quat(vec3(0.0f, 0.0f, -1.0f), dir));
    }
    vec3 localDirection() const {return _transform->localRotation() * vec3(0.0f, 0.0f, -1.0f);}
    vec3 globalDirection() const {return _transform->globalRotation() * vec3(0.0f, 0.0f, -1.0f);}
    
    void setColor(vec3 color) {_color = color;}
    vec3 color() const {return _color;}
    
    void setEnergy(float energy) {_energy = energy;}
    float energy() const {return _energy;}
    
    void setLinearAttenuation(float attenuation) {_linearAttenuation = attenuation;}
    float linearAttenuation() const {return _linearAttenuation;}
    
    void setSquareAttenuation(float attenuation) {_squareAttenuation = attenuation;}
    float squareAttenuation() const {return _squareAttenuation;}
    
    void setDistance(float distance) {_distance = distance;}
    float distance() const {return _distance;}
    
    void setConeAngle(float angle) {_coneAngle = angle;}
    float coneAngle() const {return _coneAngle;}
    
    void setSoftAngle(float angle) {_softAngle = angle;}
    float softAngle() const {return _softAngle;}
    
    void setAsDirectionalLight() {_directional = true;}
    bool isDirectionalLight() const {return _directional;}
    
    void setAsPointLight() {_directional = false;}
    bool isPointLight() const {return !_directional;}
    
    void setAsDirectionalLight(vec3 direction, vec3 color, float energy) {
      setAsDirectionalLight();
      setDirection(direction);
      setColor(color);
      setEnergy(energy);
    }
    
    void setAsPointLight(vec3 location, vec3 color, float energy) {
      setAsPointLight();
      setLocation(location);
      setColor(color);
      setEnergy(energy);
    }
    
    TransformPtr transform() {return _transform;}
  };
}

#endif /* Light_h */
