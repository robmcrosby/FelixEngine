//
//  Model.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <FelixEngine/Drawable.h>
#include <FelixEngine/Material.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/Transform.h>
#include <FelixEngine/UniformMap.h>
#include <FelixEngine/RenderItem.h>

namespace fx {
  DEFINE_OBJ_BUILDER(Model)
  
  class Model: public Drawable {
  private:
    static ModelBuilder modelBuilder;
    
  protected:
    bool _hidden;
    
    MaterialPtr _material;
    UniformsPtr _uniforms;
    Transforms  _transforms;
    RenderItem  _renderItem;
    
    std::vector<mat4> _modelTransforms;
    std::vector<mat4> _textureTransforms;
    
  public:
    Model();
    virtual ~Model();
    
    virtual bool load(const XMLTree::Node &node);
    virtual bool load(const USDCrate &crate, const std::string &path, const std::string &pass);
    virtual void update(float dt);
    
    void setHidden(bool hidden = true) {_hidden = hidden;}
    bool hidden() const {return _hidden;}
    
    bool renderable() const;
    
    void setInstances(unsigned int instances);
    unsigned int instances() const {return _renderItem.instances;}
    
    void setLayer(int layer) {_renderItem.layer = layer;}
    int layer() const {return _renderItem.layer;}
    
    void setFaceCulling(CULL_MODE mode) {_renderItem.cullMode = mode;}
    CULL_MODE faceCulling() const {return _renderItem.cullMode;}
    
    bool setMaterial(const XMLTree::Node &node);
    void setMaterial(const std::string &name);
    void setMaterial(MaterialPtr &material) {_material = material;}
    MaterialPtr material() const {return _material;}
    
    bool setMesh(const XMLTree::Node &node);
    void setMesh(const std::string &name);
    void setMesh(VertexMeshPtr &mesh) {_renderItem.mesh = mesh;}
    VertexMeshPtr mesh() const {return _renderItem.mesh;}
    void loadMesh(const std::string &file);
    
    void setScale(float scale, int index = 0);
    void setScale(const vec3 &scale, int index = 0);
    vec3 localScale(int index = 0) const;
    
    void setRotation(const quat &orientation, int index = 0);
    quat localRotation(int index = 0) const;
    
    void setLocation(const vec3 &location, int index = 0);
    vec3 localLocation(int index = 0) const;
    
    TransformPtr transform(int index = 0);
    
    void setModelTransform(const mat4 &transform, int index = 0);
    mat4 modelTransform(int index = 0);
    
    void setTextureTransform(const mat4 &transform, int index = 0);
    void setTextureTransform(const mat3 &transform, int index = 0) {setTextureTransform(mat4(transform), index);}
    mat4 textureTransform(int index = 0);
    
    mat4 globalModelTransform(int index = 0);
    mat4 localModelTransform(int index = 0);
    
  private:
    virtual bool loadXMLItem(const XMLTree::Node &node);
  };
}

#endif /* Model_h */
