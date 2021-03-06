//
//  HelloTriangle.h
//  HelloTriangle
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef HelloTriangle_h
#define HelloTriangle_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/RenderPass.h>


class HelloTriangle: public fx::Application {
private:
  fx::RenderPassPtr _renderPass;
  
public:
  HelloTriangle();
  virtual ~HelloTriangle();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* HelloTriangle_h */
