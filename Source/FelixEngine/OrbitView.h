//
//  OrbitView.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef OrbitView_h
#define OrbitView_h

#include "View.h"

#define POLAR_LIMIT 89.0f
#define DEF_POLAR_LIMIT 70.0f
#define DEF_DISTANCE 1.0f


namespace fx
{
  /**
   *
   */
  class OrbitView: public View
  {
  public:
    OrbitView(Scene *scene): View(scene), mLatitude(0), mLongitude(0), mPolarLimit(DEF_POLAR_LIMIT), mDistance(DEF_DISTANCE) {}
    virtual ~OrbitView() {}
    
    virtual void setToXml(const XMLTree::Node &node)
    {
      View::setToXml(node);
      if (node.hasSubNode("Center"))
        setCenter(node.subContents("Center"));
      if (node.hasAttribute("limit"))
        setPolarLimit(node.attributeAsFloat("limit"));
      if (node.hasAttribute("latitude"))
        setLatitude(node.attributeAsFloat("latitude"));
      if (node.hasAttribute("longitude"))
        setLongitude(node.attributeAsFloat("longitude"));
      if (node.hasAttribute("distance"))
        setDistance(node.attributeAsFloat("distance"));
    }
    
    virtual bool init()
    {
      bool success = View::init();
      if (success)
        update();
      return success;
    }
    
    void setCenter(const vec3 &center)
    {
      lock();
      mCenter = center;
      unlock();
    }
    vec3 center() const
    {
      lock();
      vec3 ret = mCenter;
      unlock();
      return ret;
    }
    
    void setLatitude(float lat)
    {
      lock();
      if (lat > mPolarLimit)
        mLatitude = mPolarLimit;
      else if (lat < -mPolarLimit)
        mLatitude = -mPolarLimit;
      else
        mLatitude = lat;
      unlock();
    }
    float latitude() const
    {
      lock();
      float ret = mLatitude;
      unlock();
      return ret;
    }
    
    void setLongitude(float lon)
    {
      lock();
      mLongitude = lon;
      unlock();
    }
    float longitude() const
    {
      lock();
      float ret = mLongitude;
      unlock();
      return ret;
    }
    
    void setPolarLimit(float limit)
    {
      lock();
      if (limit < 0.0f)
        mPolarLimit = 0.0f;
      else if (limit > POLAR_LIMIT)
        mPolarLimit = POLAR_LIMIT;
      else
        mPolarLimit = limit;
      unlock();
      setLatitude(mLatitude);
    }
    
    void setDistance(float distance)
    {
      lock();
      mDistance = distance < 0.1f ? 0.1f : distance;
      unlock();
    }
    float distance() const
    {
      lock();
      float ret = mDistance;
      unlock();
      return ret;
    }
    
  protected:
    virtual void update()
    {
      lock();
      vec3 dir = mat4::RotY(mLongitude*DegToRad) * mat4::RotX(mLatitude*DegToRad) * vec3(0.0f, 0.0f, 1.0f);
      mViewMatrix = mat4::LookAt(mCenter-dir*mDistance, mCenter, vec3(0.0f, 1.0f, 0.0f));
      unlock();
      
      View::update();
    }
    
  private:
    vec3 mCenter;
    
    float mLatitude;
    float mLongitude;
    float mPolarLimit;
    
    float mDistance;
  };
}

#endif /* OrbitView_h */
