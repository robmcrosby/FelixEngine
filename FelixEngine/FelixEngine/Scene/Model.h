//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "RenderItem.h"
#include "Scene.h"
#include "Material.h"
#include "Graphics.h"
#include "Quaternion.h"
#include "UniformMap.h"

namespace fx {
  DEFINE_OBJ_BUILDER(Model)
  
  struct Transform {
    mat4 model;
    quat rotation;
  };
  
  class Model: public RenderItem, public iObject {
  private:
    static ModelBuilder modelBuilder;
    
  private:
    Scene *_scene;
    
    MaterialPtr _material;
    VertexPtr   _mesh;
    UniformsPtr _uniforms;
    
    vec3 _scale;
    quat _orientation;
    vec3 _position;
    
    Transform _transform;
    
    bool _hidden;
    int _layer;
    
  public:
    Model();
    virtual ~Model();
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
    
    virtual void applyToTask(GraphicTask &task);
    bool setTransform(const XMLTree::Node &node);
    
    void setHidden(bool hidden = true) {_hidden = hidden;}
    bool hidden() const {return _hidden;}
    
    void setLayer(int layer) {_layer = layer;}
    int layer() const {return _layer;}
    
    bool setMaterial(const XMLTree::Node &node);
    void setMaterial(const std::string &name);
    void setMaterial(MaterialPtr &material) {_material = material;}
    MaterialPtr material() const {return _material;}
    
    bool setMesh(const XMLTree::Node &node);
    void setMesh(const std::string &name);
    void setMesh(VertexPtr &mesh) {_mesh = mesh;}
    VertexPtr mesh() const {return _mesh;}
    
    void setScale(float scale) {_scale = vec3(scale, scale, scale);}
    void setScale(const vec3 &scale) {_scale = scale;}
    vec3 scale() const {return _scale;}
    
    void setOrientation(const quat &orientation) {_orientation = orientation;}
    quat orientation() const {return _orientation;}
    
    void setPosition(const vec3 &position) {_position = position;}
    vec3 position() const {return _position;}
    
  private:
    virtual bool loadXMLItem(const XMLTree::Node &node);
  };
}

#endif /* Model_h */
