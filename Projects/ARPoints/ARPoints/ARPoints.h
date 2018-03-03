//
//  ARPoints.h
//  ARPoints
//
//  Created by Robert Crosby on 2/18/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef ARPoints_h
#define ARPoints_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/RenderPass.h>

class ARPoints: public fx::Application {
  private:
  fx::RenderScheme _renderScheme;
  fx::Scene _scene;
  fx::ModelPtr _model;
  
  public:
  ARPoints();
  virtual ~ARPoints();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* ARPoints_h */
