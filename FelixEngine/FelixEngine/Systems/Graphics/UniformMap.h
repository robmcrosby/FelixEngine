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
#include "Variant.h"
#include "GraphicResources.h"
#include <vector>
#include <map>


namespace fx {
  
  class Uniform: public Variant {
  private:
    UniformBufferPtr _buffer;

  public:
    Uniform(): Variant(), _buffer(0) {}
    virtual ~Uniform() {}

    bool usingBuffer() const {return _buffer != nullptr;}
    UniformBufferPtr buffer() const {return _buffer;}
    void useBuffer(BUFFER_COUNT bufferCount = BUFFER_TRIPLE) {
      if (!usingBuffer()) {
        _buffer = Graphics::getInstance().createUniformBuffer();
        _buffer->load(ptr(), sizeInBytes(), bufferCount);
      }
    }
    
    void update() {
      if (usingBuffer())
        _buffer->update(ptr(), sizeInBytes());
    }

    template <typename T>
    Uniform& operator=(const T &value) {setValues(&value); return *this;}

    template <typename T>
    Uniform& operator=(const std::vector<T> &values) {
      setValues(&values.at(0), values.size());
      return *this;
    }
  };
  
  
  class UniformMap;
  typedef std::shared_ptr<UniformMap> UniformsPtr;
  typedef std::list<UniformsPtr> UniformsList;
  
  class UniformMap {
  private:
    std::map<std::string, Uniform> _map;
    
  public:
    static UniformsPtr make() {return std::make_shared<UniformMap>();}
    
    UniformMap() {}
    ~UniformMap() {}
    
    std::map<std::string, Uniform>::iterator begin() {return _map.begin();}
    std::map<std::string, Uniform>::iterator end() {return _map.end();}
    
    std::map<std::string, Uniform>::const_iterator begin() const {return _map.begin();}
    std::map<std::string, Uniform>::const_iterator end() const {return _map.end();}
    
    void update() {
      for (auto uniform : _map)
        uniform.second.update();
    }
    
    Uniform& operator[](const std::string name) {
      return _map[name];
    }
  };
}

#endif /* UniformMap_h */
