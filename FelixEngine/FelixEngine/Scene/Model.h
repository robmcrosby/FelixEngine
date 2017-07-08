//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "Quaternion.h"
#include <vector>

namespace fx {
  class Material;
  class VertexMesh;
  class ShaderProgram;
  
  struct ModelData {
    mat4 model;
    quat rotation;
  };
  typedef std::vector<ModelData> InstanceModels;
  
  class Model {
  protected:
    InstanceModels _data;
    Material *_material;
    VertexMesh *_mesh;
    
  public:
    Model();
    virtual ~Model();
    
    virtual void update();
    
    void setInstances(int instances) {_data.resize(instances);}
    int instances() const {return (int)_data.size();}
    
    ModelData& data() {return _data.at(0);}
    
    void setMesh(VertexMesh *mesh) {_mesh = mesh;}
    VertexMesh* mesh() {return _mesh;}
    
    void setShader(ShaderProgram *shader);
  };
  
}

#endif /* Model_h */
