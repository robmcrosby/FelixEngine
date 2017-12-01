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
#include "Transform.h"
#include "UniformMap.h"

namespace fx {
  DEFINE_OBJ_BUILDER(Model)
  
  class Model: public RenderItem, public iObject {
  private:
    static ModelBuilder modelBuilder;
    
  private:
    Scene *_scene;
    
    MaterialPtr   _material;
    VertexMeshPtr _mesh;
    UniformsPtr   _uniforms;
    
    TransformPtr _transform;
    
    bool _hidden;
    int _layer;
    
  public:
    Model();
    virtual ~Model();
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
    
    virtual void applyToTask(GraphicTask &task);
    
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
    void setMesh(VertexMeshPtr &mesh) {_mesh = mesh;}
    VertexMeshPtr mesh() const {return _mesh;}
    
    void setScale(float scale) {_transform->setScale(scale);}
    void setScale(const vec3 &scale) {_transform->setScale(scale);}
    vec3 localScale() const {return _transform->localScale();}
    
    void setRotation(const quat &orientation) {_transform->setRotation(orientation);}
    quat localRotation() const {return _transform->localRotation();}
    
    void setLocation(const vec3 &location) {_transform->setLocation(location);}
    vec3 localLocation() const {return _transform->localLocation();}
    
    TransformPtr transform() {return _transform;}
    
  private:
    virtual bool loadXMLItem(const XMLTree::Node &node);
  };
}

#endif /* Model_h */
