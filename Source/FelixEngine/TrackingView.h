//
//  TrackingView.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef TrackingView_h
#define TrackingView_h

#include "View.h"


namespace fx
{
  /**
   *
   */
  class TrackingView: public View
  {
  public:
    TrackingView(Object *obj): View(obj), mPosition(0.0f, 0.0f, 0.0f), mTarget(0.0f, 0.0f, -1.0f), mUp(0.0f, 1.0f, 0.0f)
    {
      mUpdateDelegate = UpdateDelegate::Create<TrackingView, &TrackingView::update>(this);
    }
    virtual ~TrackingView() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = View::setToXml(node);
      if (success)
      {
        if (node->hasSubNode("Position"))
          setPosition(node->subContents("Position"));
        if (node->hasSubNode("Target"))
          setTarget(node->subContents("Target"));
        if (node->hasSubNode("Up"))
          setUp(node->subContents("Up"));
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
    
    void setPosition(const vec3 &pos) {mPosition = pos;}
    vec3 position() const {return mPosition;}
    
    void setTarget(const vec3 &target) {mTarget = target;}
    vec3 target() const {return mTarget;}
    
    void setUp(const vec3 &up) {mUp = up;}
    vec3 up() const {return mUp;}
    
  private:
    void update(void*)
    {
      setMatrix(mat4::LookAt(mPosition, mTarget, mUp));
      View::update(nullptr);
    }
    
    vec3 mPosition;
    vec3 mTarget;
    vec3 mUp;
  };
}


#endif /* TrackingView_h */
