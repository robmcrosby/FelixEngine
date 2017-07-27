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
#include "XMLTree.h"
#include <vector>

namespace fx {
  class Scene;
  class Model;
  class Material;
  class VertexMesh;
  class ShaderProgram;
  
  struct iModelBuilder {
    virtual ~iModelBuilder() {}
    virtual Model* build(Scene *scene) = 0;
  };
  typedef std::map<std::string, iModelBuilder*> ModelBuilderMap;
  
  #define DEFINE_MODEL_BUILDER(name) \
    struct name##Builder: fx::iModelBuilder { \
      static name##Builder intance; \
      name##Builder() {fx::Model::builders()[#name] = this;} \
      virtual fx::Model* build(fx::Scene *scene) {return new fx::name(scene);} \
    }; \
    name##Builder name##Builder::intance;
  
  struct ModelData {
    mat4 model;
    quat rotation;
  };
  typedef std::vector<ModelData> InstanceModels;
  
  class Model {
  protected:
    Scene *_scene;
    
    InstanceModels _data;
    Material *_material;
    VertexMesh *_mesh;
    
    float _scale;
    quat _orientation;
    vec3 _position;
    
    bool _hidden;
    int _layer;
    
  public:
    Model(Scene *scene);
    virtual ~Model();
    
    virtual void update();
    virtual bool loadXML(const XMLTree::Node &node);
    
    void setInstances(int instances) {_data.resize(instances);}
    int instances() const {return (int)_data.size();}
    
    ModelData& data() {return _data.at(0);}
    
    void setMesh(VertexMesh *mesh) {_mesh = mesh;}
    void setMesh(const std::string &name);
    VertexMesh* mesh() const {return _mesh;}
    
    void setMaterial(Material *material) {_material = material;}
    void setMaterial(const std::string &name);
    Material* material() const {return _material;}
    
    bool setTransform(const XMLTree::Node &node);
    
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
    
  public:
    static Model* build(const std::string &type, Scene *scene);
    static ModelBuilderMap& builders();
  };
  
}

#endif /* Model_h */
