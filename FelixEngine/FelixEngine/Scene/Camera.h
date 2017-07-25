//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Matrix.h"
#include "XMLTree.h"


namespace fx {
  class Scene;
  class LightRig;
  class FrameBuffer;
  class ShaderProgram;
  
  struct CameraData {
    mat4 projection;
    mat4 view;
    vec4 position;
  };
  
  class Camera {
  protected:
    Scene *_scene;
    
    CameraData _data;
    LightRig *_lightRig;
    FrameBuffer *_frame;
    ShaderProgram *_shader;
    
    bool _isClearingColor;
    vec4 _clearColor;
    
    bool _isClearingDepth;
    float _clearDepth;
    
  public:
    Camera(Scene *scene);
    virtual ~Camera();
    
    virtual void update();
    virtual bool loadXML(const XMLTree::Node &node);
    
    CameraData& data() {return _data;}
    
    void setLightRig(const std::string &name);
    void setLightRig(LightRig *rig) {_lightRig = rig;}
    LightRig* lightRig() {return _lightRig;}
    
    bool setView(const XMLTree::Node &node);
    
    void lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
      _data.view = mat4::LookAt(eye, center, up);
      _data.position = vec4(eye, 1.0);
    }
    vec3 position() const {return _data.position.xyz();}
    
    void addDepthBuffer();
    
    bool setProjection(const XMLTree::Node &node);
    void setOrthographic(float scale, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far) {
      _data.projection = mat4::Ortho(left, right, bottom, top, near, far);
    }
    
    void setFrustum(float scale, float near, float far);
    void setFrustum(float left, float right, float bottom, float top, float near, float far) {
      _data.projection = mat4::Frustum(left, right, bottom, top, near, far);
    }
    
    void setFrame(const std::string &name);
    void setFrame(FrameBuffer *frame) {_frame = frame;}
    FrameBuffer* frame() {return _frame;}
    
    void setShader(const std::string &name);
    void setShader(ShaderProgram *shader) {_shader = shader;}
    ShaderProgram* shader() {return _shader;}
    
    bool setClearState(const XMLTree::Node &node);
    
    void setClearColor(const vec4 &color = vec4(0.0f, 0.0f, 0.0f, 1.0f));
    vec4 clearColor() const {return _clearColor;}
    bool isClearingColor() const {return _isClearingColor;}
    
    void setClearDepth(float depth = 1.0f);
    float clearDepth() const {return _clearDepth;}
    bool isClearingDepth() const {return _isClearingDepth;}
  };
  
}

#endif /* Camera_h */
