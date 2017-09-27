//
//  ExampleScene.h
//  ExampleScene
//
//  Created by Robert Crosby on 7/6/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ExampleScene_h
#define ExampleScene_h

#import <FelixEngine/Application.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Model.h>

class ExampleScene: public fx::Application {
  private:
  fx::GraphicTask _task;
  fx::RenderScheme _renderScheme;
  
  fx::Scene _scene;
  fx::ModelPtr _model;
  
  public:
  ExampleScene();
  virtual ~ExampleScene();
  
  virtual void initalize();
  virtual void update(float td);
  virtual void render();
};

#endif /* ExampleScene_h */
