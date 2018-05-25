//
//  Hello3D.h
//  Hello3D
//
//  Created by Robert Crosby on 6/22/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Hello3D_h
#define Hello3D_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/Quaternion.h>


struct MVPUniform {
  fx::mat4 projection, view, model;
  fx::quat rotation;
};

class Hello3D: public fx::Application {
private:
  fx::GraphicTask _task;
  fx::UniformsPtr _uniformMap;
  fx::UniformsPtr _secondMap;
  MVPUniform _mvpUniform;
  
  fx::mat4 _matrix;
  fx::vec4 _color;
  
public:
  Hello3D();
  virtual ~Hello3D();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* Hello3D_h */
