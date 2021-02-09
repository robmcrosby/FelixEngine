//
//  Transform.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/31/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Transform_h
#define Transform_h

#include <FelixEngine/Quaternion.h>
#include <FelixEngine/XMLTree.h>
#include <memory>

namespace fx
{
  struct TransformOp {
    virtual ~TransformOp() {}
    virtual mat4 transform() const {return mat4();}
    virtual quat rotation() const {return quat();}
  };
  typedef std::shared_ptr<TransformOp> TransformOpPtr;
  //typedef std::vector<TransformOpPtr> TransformOps;

  struct TranslateOp: TransformOp {
    vec3 translation;
    TranslateOp(vec3 t = vec3()): translation(t) {}
    virtual ~TranslateOp() {}
    virtual mat4 transform() const {return mat4::Trans3d(translation);}
  };
  typedef std::shared_ptr<TranslateOp> TranslateOpPtr;

  struct ScaleOp: TransformOp {
    vec3 scale;
    ScaleOp(vec3 s = vec3(1.0f)): scale(s) {}
    virtual ~ScaleOp() {}
    virtual mat4 transform() const {return mat4::Scale(scale);}
  };
  typedef std::shared_ptr<ScaleOp> ScaleOpPtr;

  struct MatrixOp: TransformOp {
    mat4 matrix;
    MatrixOp(mat4 m = mat4()): matrix(m) {}
    virtual ~MatrixOp() {}
    virtual mat4 transform() const {return matrix;}
    virtual quat rotation() const {return matrix;}
  };
  typedef std::shared_ptr<MatrixOp> MatrixOpPtr;

  struct OrientOp: TransformOp {
    quat orientation;
    OrientOp(quat o = quat()): orientation(o) {}
    virtual ~OrientOp() {}
    virtual mat4 transform() const {return orientation.toMat4();}
    virtual quat rotation() const {return orientation;}
  };
  typedef std::shared_ptr<OrientOp> OrientOpPtr;

  struct AngleAxisOp: TransformOp {
    float angle;
    vec3 axis;
    AngleAxisOp(float r = 0, vec3 a = vec3(1.0f, 0.0f, 0.0f)): angle(r), axis(a) {}
    virtual ~AngleAxisOp() {}
    virtual mat4 transform() const {return quat(axis, angle).toMat4();}
    virtual quat rotation() const {return quat(axis, angle);}
  };
  typedef std::shared_ptr<AngleAxisOp> AngleAxisOpPtr;

  struct RotateXOp: AngleAxisOp {
    RotateXOp(float r = 0.0f): AngleAxisOp(r, vec3(1.0f, 0.0f, 0.0f)) {}
    virtual ~RotateXOp() {}
  };
  typedef std::shared_ptr<RotateXOp> RotateXOpPtr;

  struct RotateYOp: AngleAxisOp {
    RotateYOp(float r = 0.0f): AngleAxisOp(r, vec3(0.0f, 1.0f, 0.0f)) {}
    virtual ~RotateYOp() {}
  };
  typedef std::shared_ptr<RotateYOp> RotateYOpPtr;

  struct RotateZOp: AngleAxisOp {
    RotateZOp(float r = 0.0f): AngleAxisOp(r, vec3(0.0f, 0.0f, 1.0f)) {}
    virtual ~RotateZOp() {}
  };
  typedef std::shared_ptr<RotateZOp> RotateZOpPtr;

  struct RotateXYZOp: TransformOp {
    vec3 angles;
    RotateXYZOp(vec3 a = vec3()): angles(a) {}
    virtual ~RotateXYZOp() {}
    virtual mat4 transform() const {return rotation().toMat4();}
    virtual quat rotation() const {return quat::RotZ(angles.z) * quat::RotY(angles.y) * quat::RotX(angles.x);}
  };
  typedef std::shared_ptr<RotateXYZOp> RotateXYZOpPtr;

  struct RotateXZYOp: RotateXYZOp {
    RotateXZYOp(vec3 a = vec3()): RotateXYZOp(a) {}
    virtual ~RotateXZYOp() {}
    virtual quat rotation() const {return quat::RotY(angles.y) * quat::RotZ(angles.z) * quat::RotX(angles.x);}
  };
  typedef std::shared_ptr<RotateXZYOp> RotateXZYOpPtr;

  struct RotateYXZOp: RotateXYZOp {
    RotateYXZOp(vec3 a = vec3()): RotateXYZOp(a) {}
    virtual ~RotateYXZOp() {}
    virtual quat rotation() const {return quat::RotZ(angles.z) * quat::RotX(angles.x) * quat::RotY(angles.y);}
  };
  typedef std::shared_ptr<RotateYXZOp> RotateYXZOpPtr;

  struct RotateYZXOp: RotateXYZOp {
    RotateYZXOp(vec3 a = vec3()): RotateXYZOp(a) {}
    virtual ~RotateYZXOp() {}
    virtual quat rotation() const {return quat::RotX(angles.x) * quat::RotZ(angles.z) * quat::RotY(angles.y);}
  };
  typedef std::shared_ptr<RotateYZXOp> RotateYZXOpPtr;

  struct RotateZXYOp: RotateXYZOp {
    RotateZXYOp(vec3 a = vec3()): RotateXYZOp(a) {}
    virtual ~RotateZXYOp() {}
    virtual quat rotation() const {return quat::RotY(angles.y) * quat::RotX(angles.x) * quat::RotZ(angles.z);}
  };
  typedef std::shared_ptr<RotateZXYOp> RotateZXYOpPtr;

  struct RotateZYXOp: RotateXYZOp {
    RotateZYXOp(vec3 a = vec3()): RotateXYZOp(a) {}
    virtual ~RotateZYXOp() {}
    virtual quat rotation() const {return quat::RotX(angles.x) * quat::RotZ(angles.z) * quat::RotY(angles.y);}
  };
  typedef std::shared_ptr<RotateZYXOp> RotateZYXOpPtr;

  struct TransformOps: TransformOp {
    std::vector<TransformOpPtr> stack;
    
    TransformOps() {}
    virtual ~TransformOps() {}
    
    virtual mat4 transform() const {
      mat4 m;
      for (auto t : stack)
        m = t->transform() * m;
      return m;
    }
    virtual quat rotation() const {
      quat r;
      for (auto t : stack)
        r = t->rotation() * r;
      return r;
    }
    
    void add(TransformOpPtr t) {stack.push_back(t);}
    void clear() {stack.clear();}
    TransformOpPtr at(int i) const {return stack.at(i);}
    size_t size() const {return stack.size();}
    
    TranslateOpPtr addTranslation(vec3 t = vec3()) {
      TranslateOpPtr translate = std::make_shared<TranslateOp>(t);
      add(translate);
      return translate;
    }
    ScaleOpPtr addScale(vec3 s = vec3(1.0f)) {
      ScaleOpPtr scale = std::make_shared<ScaleOp>(s);
      add(scale);
      return scale;
    }
    MatrixOpPtr addMatrix(mat4 m = mat4()) {
      MatrixOpPtr matrix = std::make_shared<MatrixOp>(m);
      add(matrix);
      return matrix;
    }
    OrientOpPtr addOrientation(quat r = quat()) {
      OrientOpPtr orientation = std::make_shared<OrientOp>(r);
      add(orientation);
      return orientation;
    }
    AngleAxisOpPtr addAngleAxis(float r = 0.0f, vec3 a = vec3(1.0f, 0.0f, 0.0f)) {
      AngleAxisOpPtr angleAxis = std::make_shared<AngleAxisOp>(r, a);
      add(angleAxis);
      return angleAxis;
    }
    RotateXOpPtr addRotateX(float r = 0.0f) {
      RotateXOpPtr rotation = std::make_shared<RotateXOp>(r);
      add(rotation);
      return rotation;
    }
    RotateYOpPtr addRotateY(float r = 0.0f) {
      RotateYOpPtr rotation = std::make_shared<RotateYOp>(r);
      add(rotation);
      return rotation;
    }
    RotateZOpPtr addRotateZ(float r = 0.0f) {
      RotateZOpPtr rotation = std::make_shared<RotateZOp>(r);
      add(rotation);
      return rotation;
    }
    RotateXYZOpPtr addRotateXYZ(vec3 a = vec3()) {
      RotateXYZOpPtr rotation = std::make_shared<RotateXYZOp>(a);
      add(rotation);
      return rotation;
    }
    RotateXZYOpPtr addRotateXZY(vec3 a = vec3()) {
      RotateXZYOpPtr rotation = std::make_shared<RotateXZYOp>(a);
      add(rotation);
      return rotation;
    }
    RotateYXZOpPtr addRotateYXZ(vec3 a = vec3()) {
      RotateYXZOpPtr rotation = std::make_shared<RotateYXZOp>(a);
      add(rotation);
      return rotation;
    }
    RotateZXYOpPtr addRotateZXY(vec3 a = vec3()) {
      RotateZXYOpPtr rotation = std::make_shared<RotateZXYOp>(a);
      add(rotation);
      return rotation;
    }
    RotateZYXOpPtr addRotateZYX(vec3 a = vec3()) {
      RotateZYXOpPtr rotation = std::make_shared<RotateZYXOp>(a);
      add(rotation);
      return rotation;
    }
  };
  typedef std::shared_ptr<TransformOps> TransformOpsPtr;



  class TransformB;
  typedef std::shared_ptr<TransformB> TransformPtrB;
  typedef std::vector<TransformPtrB> TransformsB;

  class TransformB {
  private:
    TransformOps   _transformOps;
    ScaleOpPtr     _scale;
    OrientOpPtr    _orientation;
    TranslateOpPtr _translation;
    
  public:
    static TransformPtrB make() {return std::make_shared<TransformB>();}
    
    TransformB() {
      _scale = _transformOps.addScale();
      _orientation = _transformOps.addOrientation();
      _translation = _transformOps.addTranslation();
    }
    
    bool load(const XMLTree::Node &node) {
      return true;
    }
    
    mat4 transform() const {return _transformOps.transform();}
    quat rotation() const {return _transformOps.rotation();}
    
    ScaleOpPtr setScale(vec3 s = vec3(1.0f)) {
      _scale->scale = s;
      return _scale;
    }
    vec3 scale() const {return _scale->scale;}
    
    OrientOpPtr setOrientation(quat orientation = quat()) {
      _orientation->orientation = orientation;
      return _orientation;
    }
    quat orientation() const {return _orientation->orientation;}
    
    TranslateOpPtr setLocation(vec3 location = vec3()) {
      _translation->translation = location;
      return _translation;
    }
    vec3 location() const {return _translation->translation;}
  };



  class Transform;
  typedef std::shared_ptr<Transform> TransformPtr;
  typedef std::vector<TransformPtr> Transforms;
  
  class Transform {
  private:
    vec3 _scale;
    quat _rotation;
    vec3 _location;
    
    std::weak_ptr<Transform> _parrent;
    
    bool loadXMLItem(const XMLTree::Node &node) {
      if (node == "Scale") {
        setScale(node.contents());
        return true;
      }
      if (node == "Rotation") {
        setRotation(node.contents());
        return true;
      }
      if (node == "Location") {
        setLocation(node.contents());
        return true;
      }
      std::cerr << "Unknown XML " << node.element() << " in Transform" << std::endl;
      return false;
    }
    
  public:
    static TransformPtr make() {return std::make_shared<Transform>();}
    
    Transform(): _scale(1.0f, 1.0f, 1.0f) {}
    
    void setScale(float scale) {_scale = vec3(scale, scale, scale);}
    void setScale(const vec3 &scale) {_scale = scale;}
    vec3 localScale() const {return _scale;}
    
    void setRotation(const quat &rotation) {_rotation = rotation;}
    quat localRotation() const {return _rotation;}
    quat globalRotation() const {
      TransformPtr parrent = _parrent.lock();
      if (parrent)
        return parrent->globalRotation() * _rotation;
      return _rotation;
    }
    
    void setLocation(const vec3 &location) {_location = location;}
    vec3 localLocation() const {return _location;}
    vec3 globalLocation() const {
      TransformPtr parrent = _parrent.lock();
      if (parrent)
        return parrent->globalTransform() * _location;
      return _location;
    }
    
    mat4 localTransform() const {return mat4::Trans3d(_location) * _rotation.toMat4() * mat4::Scale(_scale);}
    mat4 globalTransform() const {
      TransformPtr parrent = _parrent.lock();
      if (parrent)
        return parrent->globalTransform() * localTransform();
      return localTransform();
    }
    
    TransformPtr parrent() const {return _parrent.lock();}
    void setParrent(TransformPtr parrent) {_parrent = parrent;}
    void clearParrent() {_parrent.reset();}
    
    bool load(const XMLTree::Node &node) {
      bool success = true;
      for (auto &subNode : node)
        success &= loadXMLItem(*subNode);
      return true;
    }
    
  };
}

#endif /* Transform_h */
