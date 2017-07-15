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
    vec4 position;
    vec4 color;
    vec4 factors;
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
    
    void addDirectionalLight(vec3 direction, vec3 color, float energy) {
      LightData light;
      light.position = vec4(direction.normalized(), 0.0f);
      light.color = vec4(color, energy);
      light.factors = vec4(0.0f, 0.0f, 0.0f, 0.0f);
      _lights.push_back(light);
    }
    
    void addPointLight(vec3 position, vec3 color, float energy) {
      LightData light;
      light.position = vec4(position, 1.0);
      light.color = vec4(color, energy);
      light.factors = vec4(0.0f, 0.0f, 0.0f, 0.0f);
      _lights.push_back(light);
    }
  };
}

#endif /* LightRig_h */
