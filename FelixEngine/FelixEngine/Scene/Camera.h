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


namespace fx {
  class LightRig;
  
  struct CameraData {
    mat4 projection;
    mat4 view;
    vec4 position;
  };
  
  class Camera {
  protected:
    CameraData _data;
    LightRig *_lightRig;
    
  public:
    Camera();
    virtual ~Camera();
    
    virtual void update();
    
    CameraData& data() {return _data;}
    LightRig* lightRig() {return _lightRig;}
    
    void lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
      _data.view = mat4::LookAt(eye, center, up);
      _data.position = vec4(eye, 1.0);
    }
    vec3 position() const {return _data.position.xyz();}
    
    void setOrthographic(float left, float right, float bottom, float top, float near, float far) {
      _data.projection = mat4::Ortho(left, right, bottom, top, near, far);
    }
    void setFrustum(float left, float right, float bottom, float top, float near, float far) {
      _data.projection = mat4::Frustum(left, right, bottom, top, near, far);
    }
  };
  
}

#endif /* Camera_h */
