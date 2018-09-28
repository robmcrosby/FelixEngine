//
//  ARVideo.h
//  FelixEngine
//
//  Created by Robert Crosby on 7/28/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef ARVideo_h
#define ARVideo_h

#include "Model.h"

namespace fx {
  DEFINE_OBJ_BUILDER(ARVideo)
  
  class ARVideo: public Model {
  protected:
    static ARVideoBuilder arVideoBuilder;
    
  public:
    ARVideo();
    virtual ~ARVideo();
    
    virtual bool loadXML(const XMLTree::Node &node);
    virtual void update(float td);
  };
}

#endif /* ARVideo_h */
