//
//  XMLScene.h
//  XMLScene
//
//  Created by Robert Crosby on 7/16/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef XMLScene_h
#define XMLScene_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Scene.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/GraphicTask.h>
#import <FelixEngine/RenderPass.h>
#import <FelixEngine/Transform.h>

class XMLScene: public fx::Application {
private:
  fx::RenderScheme _renderScheme;
  
  fx::Scene _scene;
  fx::TransformPtr _transform;
  
public:
  XMLScene();
  virtual ~XMLScene();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* XMLScene_h */
