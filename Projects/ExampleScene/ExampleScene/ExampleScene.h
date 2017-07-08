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
#import <FelixEngine/Scene.h>

class ExampleScene: public fx::Application {
  private:
  fx::Scene _scene;
  
  public:
  ExampleScene();
  virtual ~ExampleScene();
  
  virtual void initalize();
  virtual void update();
  virtual void render();
};

#endif /* ExampleScene_h */
