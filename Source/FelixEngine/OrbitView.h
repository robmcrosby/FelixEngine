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
    OrbitView(Object *obj): View(obj), mLatitude(0), mLongitude(0),
      mPolarLimit(DEF_POLAR_LIMIT), mDistance(DEF_DISTANCE)
    {
      mUpdateDelegate = UpdateDelegate::Create<OrbitView, &OrbitView::update>(this);
    }
    virtual ~OrbitView() {}
    
    virtual bool setToXml(const XMLTree::Node *node)
    {
      bool success = View::setToXml(node);
      if (success)
      {
        if (node->hasSubNode("Center"))
          setCenter(node->subContents("Center"));
        if (node->hasAttribute("latitude"))
          setLatitude(node->attributeAsFloat("latitude"));
        if (node->hasAttribute("longitude"))
          setLongitude(node->attributeAsFloat("longitude"));
        if (node->hasAttribute("limit"))
          setPolarLimit(node->attributeAsFloat("limit"));
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
    
    void setCenter(const vec3 &center) {mCenter = center;}
    vec3 center() const {return mCenter;}
    
    void setLatitude(float lat)
    {
      if (lat > mPolarLimit)
        mLatitude = mPolarLimit;
      else if (lat < -mPolarLimit)
        mLatitude = -mPolarLimit;
      else
        mLatitude = lat;
    }
    float latitude() const {return mLatitude;}
    
    void setLongitude(float lon)
    {
      mLongitude = lon;
    }
    float longitude() const {return mLongitude;}
    
    void setPolarLimit(float limit)
    {
      if (limit < 0.0f)
        mPolarLimit = 0.0f;
      else if (limit > POLAR_LIMIT)
        mPolarLimit = POLAR_LIMIT;
      else
        mPolarLimit = limit;
      setLatitude(mLatitude);
    }
    
    void setDistance(float distance) {mDistance = distance < 0.1f ? 0.1f : distance;}
    float distance() const {return mDistance;}
    
  private:
    void update(void*)
    {
      vec3 dir = mat4::RotY(mLongitude*DegToRad) * mat4::RotX(mLatitude*DegToRad) * vec3(0.0f, 0.0f, 1.0f);
      setMatrix(mat4::LookAt(mCenter-dir*mDistance, mCenter, vec3(0.0f, 1.0f, 0.0f)));
      
      View::update(nullptr);
    }
    
    vec3 mCenter;
    
    float mLatitude;
    float mLongitude;
    float mPolarLimit;
    
    float mDistance;
  };
}

#endif /* OrbitView_h */
