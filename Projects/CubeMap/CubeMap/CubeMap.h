//
//  CubeMap.h
//  CubeMap
//
//  Created by Robert Crosby on 11/25/20.
//

#ifndef CubeMap_h
#define CubeMap_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Quaternion.h>
#import <FelixEngine/MotionSystem.h>


struct MVPUniform {
  fx::mat4 projection, view, model;
  fx::quat rotation;
  fx::vec4 camera;
};

class CubeMap: public fx::Application {
private:
  fx::RenderPassPtr _renderPass;
  MVPUniform _mvpUniform;
  fx::quat _modelRotation;
  
public:
  CubeMap();
  virtual ~CubeMap();
  
  virtual void initalize();
  virtual void update(float td);
  virtual void handle(const fx::Event &event);
  
  void handleTouchMove(const fx::Event &event);
};

#endif /* CubeMap_h */
