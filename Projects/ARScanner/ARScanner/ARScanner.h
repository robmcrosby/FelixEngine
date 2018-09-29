//
//  ARScanner.h
//  ARScanner
//
//  Created by Robert Crosby on 9/28/18.
//  Copyright © 2018 Robert Crosby. All rights reserved.
//

#ifndef ARScanner_h
#define ARScanner_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Quaternion.h>

struct MVPUniform {
  fx::mat4 projection, view, model;
  fx::quat rotation;
};

class ARScanner: public fx::Application {
private:
  fx::RenderPassPtr _firstRenderPass;
  fx::RenderPassPtr _secondRenderPass;
  
  MVPUniform _firstUniform;
  MVPUniform _secondUniform;
  
public:
  ARScanner();
  virtual ~ARScanner();
  
  virtual void initalize();
  virtual void update(float td);
  
private:
  void setupFirstPass();
  void setupSecondPass();
};

#endif /* ARScanner_h */
