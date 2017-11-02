//
//  Transform.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/31/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Transform_h
#define Transform_h

#include "Quaternion.h"
#include "XMLTree.h"
#include <memory>

namespace fx
{
  class Transform;
  typedef std::shared_ptr<Transform> TransformPtr;
  
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
        return _rotation * parrent->globalRotation();
      return _rotation;
    }
    
    void setLocation(const vec3 &location) {_location = location;}
    vec3 localLocation() const {return _location;}
    vec3 gobalLocation() const {
      TransformPtr parrent = _parrent.lock();
      if (parrent)
        return parrent->globalTransform() * _location;
      return _location;
    }
    
    mat4 localTransform() const {return mat4::Trans3d(_location) * _rotation.toMat4() * mat4::Scale(_scale);}
    mat4 globalTransform() const {
      TransformPtr parrent = _parrent.lock();
      if (parrent)
        return localTransform() * parrent->globalTransform();
      return localTransform();
    }
    
    TransformPtr parrent() const {return _parrent.lock();}
    void setParrent(TransformPtr parrent) {_parrent = parrent;}
    void clearParrent() {_parrent.reset();}
    
    bool loadXML(const XMLTree::Node &node) {
      bool success = true;
      for (auto &subNode : node)
        success &= loadXMLItem(*subNode);
      return true;
    }
    
  };
}

#endif /* Transform_h */
