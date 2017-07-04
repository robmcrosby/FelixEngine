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

struct MVPUniform {
  fx::mat4 projection, view, model;
};

class HelloFBO: public fx::Application {
private:
  fx::GraphicTask _task;
  MVPUniform _mvpUniform;
  
public:
  HelloFBO();
  virtual ~HelloFBO();
  
  virtual void initalize();
  virtual void render();
};

#endif /* HelloFBO_h */
