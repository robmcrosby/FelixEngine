//
//  Scene.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/2/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <istream>

namespace fx
{
  class Scene
  {
  public:
    Scene() {}
    ~Scene() {}
    
    std::string name() const {return mName;}
    
  private:
    std::string mName;
  };
}

#endif /* Scene_h */
