//
//  Camera.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include <FelixEngine/Drawable.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/Transform.h>
#include <FelixEngine/GraphicStructures.h>


namespace fx {
  DEFINE_OBJ_BUILDER(Camera)

  enum PROJECTION_TYPE {
    PROJECTION_ORTHO,
    PROJECTION_FRUSTUM,
    PROJECTION_MATRIX,
  };
  
  class Camera: public Drawable {
  private:
    static CameraBuilder cameraBuilder;

  protected:
    //TransformPtr _transform;
    TransformPtr _transform;
    Texture _environmentTexture;

    PROJECTION_TYPE _projectionType;
    float _near;
    float _far;
    float _scale;
    float _angle;
    mat4 _projection;

  public:
    Camera();
    virtual ~Camera();

    virtual bool load(const XMLTree::Node &node);
    virtual void update(float dt);
    virtual STR_Camera properties(vec2 frameSize);

    bool setView(const XMLTree::Node &node);
    void setView(const mat4 &view);
    void lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
      setView(mat4::LookAt(eye, center, up).inverse());
    }
    mat4 view() const;

    void setRotation(const quat &rotation) {_transform->setOrientation(rotation);}
    quat localRotation() const {return _transform->orientation();}
    quat globalRotation() const {return _transform->orientation();}

    void setLocation(const vec3 &location) {_transform->setTranslation(location);}
    vec3 localLocation() const {return _transform->translation();}
    vec3 globalLocation() const {return _transform->translation();}

    void addDepthBuffer();

    bool setProjection(const XMLTree::Node &node);
    void setProjection(const mat4 &projection);
    void setOrthographic(float scale, float near, float far);
    void setFrustum(float angle, float near, float far);

    mat4 projection(vec2 frameSize = vec2(1.0f, 1.0f)) const;
    mat4 orthoProjection(vec2 frameSize = vec2(1.0f, 1.0f)) const;
    mat4 frustumProjection(vec2 frameSize = vec2(1.0f, 1.0f)) const;

  private:
    bool loadXMLItem(const XMLTree::Node &node);

  };
}

#endif /* Camera_h */
