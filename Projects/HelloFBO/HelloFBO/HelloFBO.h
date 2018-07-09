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
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Quaternion.h>

struct MVPUniform {
  fx::mat4 projection, view, model;
  fx::quat rotation;
};

class HelloFBO: public fx::Application {
private:
  fx::RenderPassPtr _firstRenderPass;
  fx::RenderPassPtr _secondRenderPass;
  
  MVPUniform _firstUniform;
  MVPUniform _secondUniform;
  
public:
  HelloFBO();
  virtual ~HelloFBO();
  
  virtual void initalize();
  virtual void update(float td);
  
private:
  void setupFirstPass();
  void setupSecondPass();
};

#endif /* HelloFBO_h */
