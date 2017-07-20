//
//  LightRig.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef LightRig_h
#define LightRig_h

#include "Vector.h"
#include <vector>

namespace fx {
  
  struct LightData {
    vec4 _position;
    vec4 _direction;
    vec4 _color;
    vec4 _factors;
    
    void setPosition(vec3 pos) {
      _position = vec4(pos, _position.w);
    }
    void setDirection(vec3 dir) {
      _direction = vec4(dir.normalized(), _direction.w);
    }
    
    void setColor(vec3 color) {_color = vec4(color, energy());}
    vec3 color() const {return _color.xyz();}
    
    void setEnergy(float energy) {_color.w = energy;}
    float energy() const {return _color.w;}
    
    void setLinearAttenuation(float attenuation) {_factors.x = attenuation;}
    float linearAttenuation() const {return _factors.x;}
    
    void setSquareAttenuation(float attenuation) {_factors.y = attenuation;}
    float squareAttenuation() const {return _factors.y;}
    
    void setDistance(float dist) {_direction.w = dist;}
    float distance() const {return _direction.w;}
    
    void setConeAngle(float angle) {_factors.z = angle;}
    float coneAngle() const {return _factors.z;}
    
    void setSoftAngle(float angle) {_factors.w = angle;}
    float softAngle() const {return _factors.w;}
    
    void setAsDirectionalLight() {_position.w = 0.0f;}
    void setAsDirectionalLight(vec3 direction, vec3 color, float energy) {
      setAsDirectionalLight();
      setDirection(direction);
      setColor(color);
      setEnergy(energy);
    }
    void setAsPointLight() {_position.w = 1.0f;}
    void setAsPointLight(vec3 position, vec3 color, float energy) {
      setAsPointLight();
      setPosition(position);
      setColor(color);
      setEnergy(energy);
    }
  };
  
  typedef std::vector<LightData> LightList;
  
  class LightRig {
  private:
    LightList _lights;
    
  public:
    LightRig() {}
    
    LightList& lights() {return _lights;}
    LightData& data() {return _lights.at(0);}
    size_t size() const {return _lights.size() * sizeof(LightData);}
    
    bool loadXML(const XMLTree::Node &node) {
      bool success = true;
      for (auto subNode : node)
        success &= addLight(*subNode);
      return success;
    }
    
    bool addLight(const XMLTree::Node &node) {
      if (node.hasAttributes("color", "energy")) {
        vec3 color = node.attribute("color");
        float energy = node.attributeAsFloat("energy");
        
        if (node.element() == "DirectionalLight") {
          vec3 direction = node.attribute("direction");
          addDirectionalLight(direction, color, energy);
          return true;
        }
        if (node.element() == "PointLight") {
          vec3 position = node.attribute("position");
          addPointLight(position, color, energy);
          return true;
        }
      }
      return false;
    }
    
    void addDirectionalLight(vec3 direction, vec3 color, float energy) {
      LightData light;
      light.setAsDirectionalLight(direction, color, energy);
      _lights.push_back(light);
    }
    
    void addPointLight(vec3 position, vec3 color, float energy) {
      LightData light;
      light.setAsPointLight(position, color, energy);
      _lights.push_back(light);
    }
  };
}

#endif /* LightRig_h */
