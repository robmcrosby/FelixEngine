//
//  GyroView.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/20/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef GyroView_h
#define GyroView_h

#include "View.h"
#include "FelixEngine.h"
#include "MotionSystem.h"
#include "Quaternion.h"


#define DEF_ROT_ACCELERATION 0.05f

namespace fx
{
  /**
   *
   */
  class GyroView: public View
  {
  public:
    GyroView(Scene *scene): View(scene), mDistance(4.0), mCenter(0.0f, 0.0f, 0.0f), mFwd(0.0f, 0.0f, 1.0f), mUp(1.0f, 0.0f, 0.0f),
      mFwdAxis(0.0f, 0.0f, 1.0f), mUpAxis(1.0f, 0.0f, 0.0f), mAcceleration(DEF_ROT_ACCELERATION)
    {
      setEventFlags(EVENT_MOTION);
      mMotionSystem = FelixEngine::GetMotionSystem();
      if (mMotionSystem)
        mMotionSystem->addHandler(this);
    }
    virtual ~GyroView() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      View::setToXml(node);
    }
    
    virtual bool init()
    {
      bool success = View::init();
      if (success)
        update();
      return success;
    }
    
    virtual void handle(const Event &event)
    {
      if (mActive && event == EVENT_MOTION)
      {
        quat rotation = mOrientation * event.motionData().orientation;
        rotation.rotate(event.motionData().rotation * mAcceleration);
        mFwd = rotation * mFwdAxis;
        mUp = rotation * mUpAxis;
      }
    }
    
    void setDistance(float distance) {mDistance = distance < 0.1f ? 0.1f : distance;}
    float distance() const {return mDistance;}
    
    void setAcceleration(float acceleration) {mAcceleration = acceleration;}
    float acceleration() const {return mAcceleration;}
    
    void setForwardAxis(const vec3 &axis) {mFwdAxis = axis;}
    void setUpAxis(const vec3 &axis) {mUpAxis = axis;}
    
    void setOrientation(const quat &orient) {mOrientation = orient;}
    quat orientation() const {return mOrientation;}
    
  private:
    virtual void update()
    {
      vec3 pos(mCenter + mFwd*mDistance);
      setMatrix(mat4::LookAt(pos, mCenter, mUp));
      View::update();
    }
    
    vec3 mCenter;
    vec3 mFwdAxis, mUpAxis;
    vec3 mFwd, mUp;
    quat mOrientation;
    
    float mDistance;
    float mAcceleration;
    MotionSystem *mMotionSystem;
  };
}

#endif /* GyroView_h */
