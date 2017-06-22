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

class Hello3D: public fx::Application {
private:
  fx::GraphicTask _task;
  
public:
  Hello3D();
  virtual ~Hello3D();
  
  virtual void start();
  virtual void update();
};

#endif /* Hello3D_h */
