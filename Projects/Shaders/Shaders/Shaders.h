//
//  Shaders.h
//  Shaders
//
//  Created by Robert Crosby on 12/2/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Shaders_h
#define Shaders_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Transform.h>

class Shaders: public fx::Application {
private:
  fx::RenderScheme _renderScheme;
  
  fx::Scene _scene;
  fx::TransformPtr _transform;
  
public:
  Shaders();
  virtual ~Shaders();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* Shaders_hpp */
