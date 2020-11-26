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
};

class CubeMap: public fx::Application {
private:
  fx::RenderPassPtr _renderPass;
  MVPUniform _mvpUniform;
  
public:
  CubeMap();
  virtual ~CubeMap();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* CubeMap_h */
