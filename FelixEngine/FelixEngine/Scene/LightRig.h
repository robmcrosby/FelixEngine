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
  
  class LighRig {
  private:
    LightList _lights;
    
  public:
    LightRig() {}
    
    LightList& lights() {return _lights;}
  };
}

#endif /* LightRig_h */
