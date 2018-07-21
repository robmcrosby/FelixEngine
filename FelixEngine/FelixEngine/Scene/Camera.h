//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Scene.h"
#include "Graphics.h"
#include "UniformMap.h"
#include "Transform.h"
#include "GraphicStructures.h"
#include "RenderPass.h"


namespace fx {
  DEFINE_OBJ_BUILDER(Camera)

  enum PROJECTION_TYPE {
    PROJECTION_ORTHO,
    PROJECTION_FRUSTUM,
    PROJECTION_MATRIX,
  };
  
  class Camera: public iObject {
  private:
    static CameraBuilder cameraBuilder;

  protected:
    Scene *_scene;

    TransformPtr _transform;
    RenderPasses _renderPasses;

    PROJECTION_TYPE _projectionType;
    float _near;
    float _far;
    float _scale;
    float _angle;
    mat4 _projection;

  public:
    Camera();
    virtual ~Camera();

    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
    virtual STR_Camera properties(vec2 frameSize);

    bool setView(const XMLTree::Node &node);
    void setView(const mat4 &view);
    void lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
      setView(mat4::LookAt(eye, center, up));
    }
    mat4 view() const;

    void setRotation(const quat &rotation) {_transform->setRotation(rotation);}
    quat localRotation() const {return _transform->localRotation();}
    quat globalRotation() const {return _transform->globalRotation();}

    void setLocation(const vec3 &location) {_transform->setLocation(location);}
    vec3 localLocation() const {return _transform->localLocation();}
    vec3 globalLocation() const {return _transform->globalLocation();}

    void addDepthBuffer();

    bool setProjection(const XMLTree::Node &node);
    void setProjection(const mat4 &projection);
    void setOrthographic(float scale, float near, float far);
    void setFrustum(float angle, float near, float far);

    mat4 projection(vec2 frameSize) const;
    mat4 orthoProjection(vec2 frameSize) const;
    mat4 frustumProjection(vec2 frameSize) const;
    
    bool setToRenderPass(const XMLTree::Node &node);
    void setToRenderPass(const std::string &name);
    void setToRenderPass(RenderPassPtr pass);
    
    void removeFromRenderPass(const std::string &name);
    void removeFromRenderPass(RenderPassPtr pass);

  private:
    bool loadXMLItem(const XMLTree::Node &node);

  };
}

#endif /* Camera_h */
