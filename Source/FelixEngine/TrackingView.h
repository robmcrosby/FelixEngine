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
    TrackingView(Scene *scene): View(scene), mPosition(0.0f, 0.0f, 0.0f), mTarget(0.0f, 0.0f, -1.0f), mUp(0.0f, 1.0f, 0.0f) {}
    virtual ~TrackingView() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      View::setToXml(node);
      if (node.hasSubNode("Position"))
        setPosition(node.subContents("Position"));
      if (node.hasSubNode("Target"))
        setTarget(node.subContents("Target"));
      if (node.hasSubNode("Up"))
        setUp(node.subContents("Up"));
    }
    
    virtual bool init()
    {
      bool success = View::init();
      if (success)
        update();
      return success;
    }
    
    void setPosition(const vec3 &pos) {mPosition = pos;}
    vec3 position() const {return mPosition;}
    
    void setTarget(const vec3 &target) {mTarget = target;}
    vec3 target() const {return mTarget;}
    
    void setUp(const vec3 &up) {mUp = up;}
    vec3 up() const {return mUp;}
    
  private:
    virtual void update()
    {
      setMatrix(mat4::LookAt(mPosition, mTarget, mUp));
      View::update();
    }
    
  private:
    vec3 mPosition;
    vec3 mTarget;
    vec3 mUp;
  };
}


#endif /* TrackingView_h */
