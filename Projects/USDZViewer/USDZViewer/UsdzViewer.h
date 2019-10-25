//
//  UsdzViewer.h
//  USDZViewer
//
//  Created by Robert Crosby on 10/24/19.
//  Copyright © 2019 Robert Crosby. All rights reserved.
//

#ifndef UsdzViewer_h
#define UsdzViewer_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>
#import <FelixEngine/RenderPass.h>


class UsdzViewer: public fx::Application {
private:
  fx::RenderPassPtr _renderPass;
  
public:
  UsdzViewer();
  virtual ~UsdzViewer();
  
  virtual void initalize();
  virtual void update(float td);
};

#endif /* UsdzViewer_h */
