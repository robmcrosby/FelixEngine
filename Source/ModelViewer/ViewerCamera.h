//
//  ViewerCamera.h
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#ifndef ViewerCamera_h
#define ViewerCamera_h

#include <FelixEngine/Camera.h>


class ViewerCamera: public fx::Camera
{
public:
  ViewerCamera(fx::Scene *scene);
  virtual ~ViewerCamera() {}
};

#endif /* ViewerCamera_h */
