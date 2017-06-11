//
//  IOSMotionSystem.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/11/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef IOSMotionSystem_h
#define IOSMotionSystem_h

#include "MotionSystem.h"

#define DEFAULT_FREQUENCY 10.0f


namespace fx
{
  class MotionInfo;
  
  class IOSMotionSystem: public MotionSystem
  {
  public:
    IOSMotionSystem();
    virtual ~IOSMotionSystem();
    
    virtual bool setToXml(const XMLTree::Node *node);
    virtual bool init();
    
    void setUpdateFrequency(float frequency);
    float updateFrequency() const {return mUpdateFrequecy;}
    
    void handleMotionData(vec3 gravity, vec3 acceleration, vec3 rotationRate, quat rotation);
    
    static IOSMotionSystem *Instance;
    
  private:
    float mUpdateFrequecy;
    MotionInfo *mInfo;
  };
}

#endif /* IOSMotionSystem_hpp */
