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
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/Model.h>
#import <FelixEngine/Camera.h>

class ExampleScene: public fx::Application {
  private:
  fx::GraphicTask _task;
  fx::Model  _model;
  fx::Camera _camera;
  
  public:
  ExampleScene();
  virtual ~ExampleScene();
  
  virtual void initalize();
  virtual void render();
};

#endif /* ExampleScene_h */
