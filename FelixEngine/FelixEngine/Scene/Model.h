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
    
    float _scale;
    quat _orientation;
    vec3 _position;
    
    bool _hidden;
    int _layer;
    
  public:
    Model();
    virtual ~Model();
    
    virtual void update();
    
    void setInstances(int instances) {_data.resize(instances);}
    int instances() const {return (int)_data.size();}
    
    ModelData& data() {return _data.at(0);}
    
    void setMesh(VertexMesh *mesh) {_mesh = mesh;}
    VertexMesh* mesh() const {return _mesh;}
    
    void setMaterial(Material *material) {_material = material;}
    Material* material() const {return _material;}
    
    void setShader(ShaderProgram *shader);
    void enableDepthTesting();
    
    void setHidden(bool hidden = true) {_hidden = hidden;}
    bool hidden() const {return _hidden;}
    
    void setLayer(int layer) {_layer = layer;}
    int layer() const {return _layer;}
    
    void setScale(float scale) {_scale = scale;}
    float scale() const {return _scale;}
    
    void setOrientation(const quat &orientation) {_orientation = orientation;}
    quat orientation() const {return _orientation;}
    
    void setPosition(const vec3 &position) {_position = position;}
    vec3 position() const {return _position;}
  };
  
}

#endif /* Model_h */
