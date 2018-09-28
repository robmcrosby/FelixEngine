//
//  HelloTexture.h
//  HelloTexture
//
//  Created by Robert Crosby on 7/1/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef HelloTexture_h
#define HelloTexture_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/RenderPass.h>

struct MVPUniform {
  fx::mat4 projection, view, model;
};

class HelloTexture: public fx::Application {
private:
  fx::RenderPassPtr _renderPass;
  MVPUniform _mvpUniform;
  
public:
  HelloTexture();
  virtual ~HelloTexture();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* HelloTexture_h */
