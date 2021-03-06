//
//  XMLScene.h
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef XMLScene_h
#define XMLScene_h

#include <FelixEngine/Application.h>
#include <FelixEngine/Scene.h>
#include <FelixEngine/Graphics.h>
#include <FelixEngine/RenderPass.h>
#include <FelixEngine/Camera.h>
#include <FelixEngine/Model.h>
#include <FelixEngine/Transform.h>

class XMLScene: public fx::Application {
private:
  fx::Scene _scene;
  fx::TransformPtr _transform;
  
  fx::ModelPtr _model;
  fx::CameraPtr _camera;
  
public:
  XMLScene();
  virtual ~XMLScene();
  
  virtual void initalize();
  virtual void update(float td);
  virtual void handle(const fx::Event &event);
  
  void handleTouchMove(const fx::Event &event);
};

#endif /* XMLScene_h */
