//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "RenderItem.h"
#include "Scene.h"
#include "Graphics.h"
#include "UniformMap.h"


namespace fx {
  DEFINE_OBJ_BUILDER(Camera)
  
  struct CameraProperties {
    mat4 projection;
    mat4 view;
    vec4 position;
  };

  
  class Camera: public RenderItem, public iObject {
  private:
    static CameraBuilder cameraBuilder;
    
  protected:
    Scene *_scene;
    
    CameraProperties _properties;
    UniformsPtr _uniforms;
    
    FramePtr  _frame;
    ShaderPtr _shader;
    
    bool _isClearingColor;
    vec4 _clearColor;
    
    bool _isClearingDepth;
    float _clearDepth;
    
  public:
    Camera();
    virtual ~Camera();
    
    virtual void setScene(Scene *scene) {_scene = scene;}
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float dt);
    
    virtual void setupTemplateTask(GraphicTask &task);
    virtual void applyToTask(GraphicTask &task);
    
    CameraProperties& properties() {return _properties;}
    
    bool setView(const XMLTree::Node &node);
    void setView(const mat4 &view) {
      _properties.view = view;
      _properties.position = view * vec4(0.0f, 0.0f, 0.0f, 1.0f);
      _properties.position /= _properties.position.w;
    }
    void lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
      _properties.view = mat4::LookAt(eye, center, up);
      _properties.position = vec4(eye, 1.0);
    }
    vec3 position() const {return _properties.position.xyz();}
    
    void addDepthBuffer();
    
    bool setProjection(const XMLTree::Node &node);
    void setProjection(const mat4 &projection) {_properties.projection = projection;}
    void setOrthographic(float scale, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far) {
      _properties.projection = mat4::Ortho(left, right, bottom, top, near, far);
    }
    
    void setFrustum(float scale, float near, float far);
    void setFrustum(float left, float right, float bottom, float top, float near, float far) {
      _properties.projection = mat4::Frustum(left, right, bottom, top, near, far);
    }
    
    bool setFrame(const XMLTree::Node &node);
    void setFrame(const std::string &name);
    void setFrame(FramePtr &frame) {_frame = frame;}
    FramePtr frame() {return _frame;}
    
    bool setShader(const XMLTree::Node &node);
    void setShader(const std::string &name);
    void setShader(ShaderPtr &shader) {_shader = shader;}
    ShaderPtr shader() {return _shader;}
    
    bool setClearState(const XMLTree::Node &node);
    
    void setClearColor(const vec4 &color = vec4(0.0f, 0.0f, 0.0f, 1.0f));
    vec4 clearColor() const {return _clearColor;}
    bool isClearingColor() const {return _isClearingColor;}
    
    void setClearDepth(float depth = 1.0f);
    float clearDepth() const {return _clearDepth;}
    bool isClearingDepth() const {return _isClearingDepth;}
    
  private:
    bool loadXMLItem(const XMLTree::Node &node);
    
  };
}

#endif /* Camera_h */
