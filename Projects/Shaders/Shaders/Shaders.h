//
//  Shaders.h
//  Shaders
//
//  Created by Robert Crosby on 12/2/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef Shaders_h
#define Shaders_h

#include <FelixEngine/Application.h>
#include <FelixEngine/Scene.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/RenderPass.h>
#include <FelixEngine/Transform.h>
#include <FelixEngine/Model.h>

class Shaders: public fx::Application {
private:
  fx::Scene _scene;
  std::vector<fx::ModelPtr> _models;
  fx::quat _rotation;
  
public:
  Shaders();
  virtual ~Shaders();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* Shaders_hpp */
