//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "Material.h"
#include "Quaternion.h"
#include <vector>

namespace fx {
  class Scene;
  
  struct ModelData {
    mat4 model;
    quat rotation;
  };
  typedef std::vector<ModelData> InstanceModels;
  
  class Model {
  private:
    Scene *_scene;
    
  protected:
    InstanceModels _data;
    Material _material;
    VertexMesh *_mesh;
    
  public:
    Model(Scene *scene): _scene(scene), _data(1) {}
    virtual ~Model() {}
    
    void setInstances(int instances) {_data.resize(instances);}
    int instances() const {return (int)_data.size();}
    
    ModelData& data() {return _data.at(0);}
    
    void setMesh(VertexMesh *mesh) {_mesh = mesh;}
    VertexMesh* mesh() {return _mesh;}
  };
  
}

#endif /* Model_h */
