//
//  UniformMap.h
//  FelixEngine
//
//  Created by Robert Crosby on 6/29/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef UniformMap_h
#define UniformMap_h

#include "Graphics.h"
#include "GraphicResources.h"
#include <map>


namespace fx {
  class UniformMap {
  private:
    std::map<std::string, UniformBuffer*> _map;
    
  public:
    UniformMap() {}
    ~UniformMap() {}
    
    std::map<std::string, UniformBuffer*>::iterator begin() {return _map.begin();}
    std::map<std::string, UniformBuffer*>::iterator end() {return _map.end();}
    
    std::map<std::string, UniformBuffer*>::const_iterator begin() const {return _map.begin();}
    std::map<std::string, UniformBuffer*>::const_iterator end() const {return _map.end();}
    
    UniformBuffer& operator[](const std::string name) {
      if (_map.count(name) == 0)
        _map[name] = Graphics::getInstance().createUniformBuffer();
      return *_map.at(name);
    }
  };
}

#endif /* UniformMap_h */
