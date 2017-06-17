//
//  HelloTriangle.h
//  HelloTriangle
//
//  Created by Robert Crosby on 6/14/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef HelloTriangle_h
#define HelloTriangle_h

#import <FelixEngine/Application.h>
#import <FelixEngine/Graphics.h>

class HelloTriangle: public fx::Application {
private:
  fx::FrameBuffer   *_window;
  fx::ShaderProgram *_shader;
  fx::VertexMesh    *_mesh;
  
public:
  HelloTriangle();
  virtual ~HelloTriangle();
  
  virtual void start();
};

#endif /* HelloTriangle_h */
