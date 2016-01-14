//
//  Transform.h
//  FelixEngine
//
//  Created by Robert Crosby on 10/9/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef Transform_h
#define Transform_h

#include "Component.h"
#include "RenderSlots.h"
#include "Matrix.h"

namespace fx
{
  /**
   *
   */
  class Transform: public Component
  {
  public:
    enum TYPE
    {
      SCALE,
      ANGLE_AXIS,
      EULER_XYZ,
      EULER_XZY,
      EULER_YXZ,
      EULER_YZX,
      EULER_ZXY,
      EULER_ZYX,
      QUATERNION,
      TRANSLATE,
      IDENITY,
    };
    
    static TYPE GetType(const std::string &str)
    {
      if (str == "Scale")
        return SCALE;
      if (str == "Axis")
        return ANGLE_AXIS;
      if (str == "Euler")
        return EULER_XYZ;
      if (str == "Quaternion")
        return QUATERNION;
      if (str == "Translate")
        return TRANSLATE;
      return IDENITY;
    }
    
    static TYPE GetEuler(const std::string &str)
    {
      if (str == "xyz")
        return EULER_XYZ;
      if (str == "xzy")
        return EULER_XZY;
      if (str == "yxz")
        return EULER_YXZ;
      if (str == "yzx")
        return EULER_YZX;
      if (str == "zxy")
        return EULER_ZXY;
      if (str == "zyx")
        return EULER_ZYX;
      return EULER_XYZ;
    }
    
  public:
    struct Item
    {
      Item(TYPE t = IDENITY, vec4 d = vec4()): type(t), data(d) {}
      Item& operator=(TYPE t) {type = t; return *this;}
      Item& operator=(const vec4 &d) {data = d; return *this;}
      bool operator==(TYPE t) const {return type == t;}
      void apply(mat4 &matrix) const {matrix *= toMatrix4x4();}
      void setToXML(const XMLTree::Node *node)
      {
        type = GetType(node->element());
        if (type == EULER_XYZ && node->hasAttribute("order"))
          type = GetEuler(node->attribute("order"));
        
        if (type == ANGLE_AXIS)
          data = vec4(vec3(node->contents()), node->attributeAsFloat("angle"));
        else if (type == QUATERNION)
          data = vec4(node->contents());
        else
          data = vec4(vec3(node->contents()), 0.0f);
      }
      mat4 toMatrix4x4() const
      {
        switch (type)
        {
          case SCALE:
            return mat4::Scale(data.xyz());
          case ANGLE_AXIS:
            return quat(data.xyz(), data.w).toMat4();
          case QUATERNION:
            return quat(data).toMat4();
          case TRANSLATE:
            return mat4::Trans3d(data.xyz());
          case EULER_XYZ:
            return mat4::RotX(data.x*DegToRad) * mat4::RotY(data.y*DegToRad) * mat4::RotZ(data.z*DegToRad);
          case EULER_XZY:
            return mat4::RotX(data.x) * mat4::RotZ(data.z) * mat4::RotY(data.y);
          case EULER_YXZ:
            return mat4::RotY(data.y) * mat4::RotX(data.x) * mat4::RotZ(data.z);
          case EULER_YZX:
            return mat4::RotY(data.y) * mat4::RotZ(data.z) * mat4::RotX(data.x);
          case EULER_ZXY:
            return mat4::RotZ(data.z) * mat4::RotX(data.x) * mat4::RotY(data.y);
          case EULER_ZYX:
            return mat4::RotZ(data.z) * mat4::RotY(data.y) * mat4::RotX(data.x);
          case IDENITY:
            return mat4();
        }
        return mat4();
      }
      
      vec4 data;
      TYPE type;
    };
    typedef std::list<Item> Stack;
    
    typedef Stack::iterator iterator;
    iterator begin() {return mStack.begin();}
    iterator end() {return mStack.end();}
    
    typedef Stack::const_iterator const_iterator;
    const_iterator begin() const {return mStack.begin();}
    const_iterator end() const {return mStack.end();}
    
    void clear() {mStack.clear();}
    void pushBack(const Item &item) {mStack.push_back(item);}
    void pushFront(const Item &item) {mStack.push_front(item);}
    
    void addScale(const vec3 &scale) {pushBack(Item(SCALE, vec4(scale, 1.0f)));}
    void addScale(const vec2 &scale) {pushBack(Item(SCALE, vec4(scale, 1.0f, 1.0f)));}
    
    void addTranslate(const vec3 &trans) {pushBack(Item(TRANSLATE, vec4(trans, 1.0f)));}
    void addTranslate(const vec2 &trans) {pushBack(Item(TRANSLATE, vec4(trans, 0.0f, 1.0f)));}
    
    Item& back() {return mStack.back();}
    Item& front() {return mStack.front();}
    
  public:
    Transform(Scene *scene): Component(scene), mRenderSlots(0), mLock(0) {}
    virtual ~Transform() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      Component::setToXml(node);
      for (XMLTree::const_iterator itr = node.begin(); itr != node.end(); ++itr)
      {
        pushBack(IDENITY);
        back().setToXML(*itr);
      }
    }
    
    virtual bool init()
    {
      mRenderSlots = getSibling<RenderSlots>();
      updateMatrices();
      return Component::init() && mRenderSlots;
    }
    
    void updateMatrices()
    {
      lock();
      
      mModelMatrix = mat4();
      mRotationMatrix = mat4();
      for (const_iterator itr = begin(); itr != end(); ++itr)
      {
        itr->apply(mModelMatrix);
        if (itr->type != SCALE && itr->type != TRANSLATE)
          itr->apply(mRotationMatrix);
      }
      unlock();
    }
    
  protected:
    virtual void update()
    {
      updateMatrices();
//      lock();
//      if (mRenderSlots)
//      {
//        for (RenderSlots::iterator itr = mRenderSlots->begin(); itr != mRenderSlots->end(); ++itr)
//        {
//          (*itr)->uniforms().set("Model", mModelMatrix);
//          (*itr)->uniforms().set("Rotation", mRotationMatrix);
//        }
//      }
//      unlock();
      Component::update();
    }
    
  private:
    Stack mStack;
    mat4 mModelMatrix;
    mat4 mRotationMatrix;
    
    mutable SDL_SpinLock mLock;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Transform_h */
