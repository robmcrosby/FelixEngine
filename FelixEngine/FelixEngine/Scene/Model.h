//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

//#include "RenderItem.h"
//#include "Scene.h"
//#include "Material.h"
//#include "Graphics.h"
//#include "Transform.h"
//#include "UniformMap.h"
//
//namespace fx {
//  DEFINE_OBJ_BUILDER(Model)
//  
//  class Model: public RenderItem, public iObject {
//  private:
//    static ModelBuilder modelBuilder;
//    
//  private:
//    Scene *_scene;
//    
//    MaterialPtr   _material;
//    VertexMeshPtr _mesh;
//    UniformsPtr   _uniforms;
//    
//    unsigned int _instances;
//    Transforms _transforms;
//    
//    bool _hidden;
//    int _layer;
//    
//    CULL_MODE _cullMode;
//    
//  public:
//    Model();
//    virtual ~Model();
//    
//    virtual void setScene(Scene *scene) {_scene = scene;}
//    virtual bool loadXML(const XMLTree::Node &node);
//    virtual void update(float dt);
//    
//    virtual void applyToTask(GraphicTask &task);
//    
//    void setHidden(bool hidden = true) {_hidden = hidden;}
//    bool hidden() const {return _hidden;}
//    
//    void setInstances(unsigned int instances);
//    unsigned int instances() const {return _instances;}
//    
//    void setLayer(int layer) {_layer = layer;}
//    int layer() const {return _layer;}
//    
//    void setFaceCulling(CULL_MODE mode) {_cullMode = mode;}
//    CULL_MODE faceCulling() const {return _cullMode;}
//    
//    bool setMaterial(const XMLTree::Node &node);
//    void setMaterial(const std::string &name);
//    void setMaterial(MaterialPtr &material) {_material = material;}
//    MaterialPtr material() const {return _material;}
//    
//    bool setMesh(const XMLTree::Node &node);
//    void setMesh(const std::string &name);
//    void setMesh(VertexMeshPtr &mesh) {_mesh = mesh;}
//    VertexMeshPtr mesh() const {return _mesh;}
//    
//    void setScale(float scale, int index = 0);
//    void setScale(const vec3 &scale, int index = 0);
//    vec3 localScale(int index = 0) const;
//    
//    void setRotation(const quat &orientation, int index = 0);
//    quat localRotation(int index = 0) const;
//    
//    void setLocation(const vec3 &location, int index = 0);
//    vec3 localLocation(int index = 0) const;
//    
//    TransformPtr transform(int index = 0);
//    
//  private:
//    virtual bool loadXMLItem(const XMLTree::Node &node);
//  };
//}

#endif /* Model_h */
