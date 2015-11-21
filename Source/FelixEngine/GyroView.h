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


namespace fx
{
  /**
   *
   */
  class GyroView: public View
  {
  public:
    GyroView(Object *obj): View(obj), mDistance(4.0), mCenter(0.0f, 0.0f, 0.0f), mDirection(0.0f, 0.0f, -1.0f), mUp(0.0f, 1.0f, 0.0f)
    {
      setEventFlags(EVENT_MOTION_COMBINED);
      mUpdateDelegate = UpdateDelegate::Create<GyroView, &GyroView::update>(this);
      mMotionSystem = FelixEngine::GetMotionSystem();
      if (mMotionSystem)
        mMotionSystem->addHandler(this);
    }
    virtual ~GyroView() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = View::setToXml(node);
      if (success)
      {
      }
      return success;
    }
    
    virtual bool init()
    {
      bool success = View::init();
      if (success)
        update(nullptr);
      return success;
    }
    
    virtual void handle(const Event &event)
    {
      if (event == EVENT_MOTION_COMBINED)
      {
        if (mInverseRotation == quat())
          mInverseRotation = quat(vec3(0.0, 1.0, 0.0), 180.0f * DegToRad) * event.motionData().rotation.inverse();
        
        quat rotation = mInverseRotation * event.motionData().rotation;
        mDirection = rotation * vec3(0.0f, 0.0f, 1.0f);
        mUp = rotation * vec3(0.0f, 1.0f, 0.0f);
      }
    }
    
  private:
    void update(void*)
    {
      vec3 pos(mCenter + mDirection*mDistance);
      setMatrix(mat4::LookAt(pos, mCenter, mUp));
      View::update(nullptr);
    }
    
    vec3 mCenter;
    vec3 mDirection;
    vec3 mUp;
    quat mInverseRotation;
    
    float mDistance;
    MotionSystem *mMotionSystem;
  };
}

#endif /* GyroView_h */
