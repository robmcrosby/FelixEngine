//
//  ViewerCamera.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 11/18/15.
//  Copyright © 2015 Robert Crosby. All rights reserved.
//

#include "ViewerCamera.h"


using namespace std;

DEFINE_OBJECT_ID(ViewerCamera)

ViewerCamera::ViewerCamera(fx::Scene *scene): fx::Camera(scene)
{
  cout << "Created Viewer Camera" << endl;
}
