//
//  HelloFBO.h
//  HelloFBO
//
//  Created by Robert Crosby on 7/4/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef HelloFBO_h
#define HelloFBO_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/Quaternion.h>

struct MVPUniform {
  fx::mat4 projection, view, model;
  fx::quat rotation;
};

class HelloFBO: public fx::Application {
private:
  fx::GraphicTask _firstTask;
  MVPUniform _firstUniform;
  
  fx::GraphicTask _secondTask;
  MVPUniform _secondUniform;
  
public:
  HelloFBO();
  virtual ~HelloFBO();
  
  virtual void initalize();
  virtual void update();
  virtual void render();
  
private:
  void setupFirstTask();
  void setupSecondTask();
};

#endif /* HelloFBO_h */
