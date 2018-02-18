//
//  ModelViewer.h
//  ModelViewer
//
//  Created by Robert Crosby on 12/21/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ModelViewer_h
#define ModelViewer_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Transform.h>

class ModelViewer: public fx::Application {
private:
  fx::RenderScheme _renderScheme;
  
  fx::Scene _scene;
  fx::TransformPtr _transform;
  
public:
  ModelViewer();
  virtual ~ModelViewer();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* ModelViewer_h */
