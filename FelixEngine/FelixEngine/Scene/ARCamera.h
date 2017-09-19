//
//  ARCamera.h
//  FelixEngine
//
//  Created by Robert Crosby on 8/5/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#ifndef ARCamera_h
#define ARCamera_h

#include "Camera.h"
#include "GraphicTask.h"

namespace fx {
  class ARCamera: public Camera {
  private:
    GraphicTask _task;
    TextureMap _textureMap;
    TexturePtr _imageY;
    TexturePtr _imageCbCr;
    
    mat4 _imageTransform;
    UniformMap _uniformMap;
    
  private:
    void setupPreDraw();
    bool cameraImagesReady();
    
  public:
    ARCamera(Scene *scene);
    virtual ~ARCamera();
    
    virtual void update();
    virtual bool preDraw();
  };
}

#endif /* ARCamera_h */
