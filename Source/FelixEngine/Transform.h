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
#include "Matrix.h"

namespace fx
{
  class RenderSlots;
  
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
    static TYPE GetType(const std::string &str);
    static TYPE GetEuler(const std::string &str);
    
  public:
    struct Item
    {
      Item(TYPE t = IDENITY, vec4 d = vec4()): type(t), data(d) {}
      Item& operator=(TYPE t) {type = t; return *this;}
      Item& operator=(const vec4 &d) {data = d; return *this;}
      bool operator==(TYPE t) const {return type == t;}
      void apply(mat4 &matrix) const {matrix *= toMatrix4x4();}
      bool setToXML(const XMLTree::Node *node);
      mat4 toMatrix4x4() const;
      
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
    
    Item& back() {return mStack.back();}
    Item& front() {return mStack.front();}
    
  public:
    Transform(Object *obj);
    virtual ~Transform();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    void updateMatrices();
    
    void lock() {SDL_AtomicLock(&mLock);}
    void unlock() {SDL_AtomicUnlock(&mLock);}
    
  private:
    void update(void*);
    
    Stack mStack;
    mat4 mModelMatrix;
    mat4 mRotationMatrix;
    
    mutable SDL_SpinLock mLock;
    RenderSlots *mRenderSlots;
  };
}

#endif /* Transform_h */
