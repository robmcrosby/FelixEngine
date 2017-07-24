//
//  XMLScene.h
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef XMLScene_h
#define XMLScene_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>

class XMLScene: public fx::Application {
private:
  fx::Scene _scene;
  fx::Model *_model;
  
  //fx::GraphicTask _task;
  
public:
  XMLScene();
  virtual ~XMLScene();
  
  virtual void initalize();
  virtual void update();
  virtual void render();
};

#endif /* XMLScene_h */
